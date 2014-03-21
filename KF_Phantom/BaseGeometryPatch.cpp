#include "StdAfx.h"
#include "BaseGeometryPatch.h"


BaseGeometryPatch::BaseGeometryPatch(void)
{
	hHapticEffect = NULL;
	patchVec.set(0, 1, 0);
	patchPos.set(0, 0, 0);
	patchRadius = 10;
	stiffnessK = 1.0;

	phantomToSim = NULL;
	simToPhantom = NULL;
}


BaseGeometryPatch::~BaseGeometryPatch(void)
{
}

bool BaseGeometryPatch::setup()
{
	/* Initialize the device, must be done before attempting to call any hd 
	   functions. Passing in HD_DEFAULT_DEVICE causes the default device to be 
	   initialized. */
	hHD = hdInitDevice(HD_DEFAULT_DEVICE);
	if (HD_DEVICE_ERROR(error = hdGetError())) 
	{
		hduPrintError(stderr, &error, "Failed to initialize haptic device");
		fprintf(stderr, "\nPress any key to quit.\n");
		getch();
		return false;
	}

	printf("Hello Haptic Device!\n");
	printf("Found device model: %s.\n\n", hdGetString(HD_DEVICE_MODEL_TYPE));

	/* Schedule the main callback that will render forceVecs to the device. */
	hHapticEffect = hdScheduleAsynchronous(patchCallback, this, HD_DEFAULT_SCHEDULER_PRIORITY);

	return true;
}

bool BaseGeometryPatch::run()
{
	/* 
	Wait until the user presses a key.  Meanwhile, the scheduler
	runs and applies forceVecs to the device. 
	*/

	hdEnable(HD_FORCE_OUTPUT);
	hdStartScheduler();

	/* Check for errors and abort if so. */
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "Failed to start scheduler");
		fprintf(stderr, "\nPress CR to quit.\n");
		return false;
	}



	return true;
}

bool BaseGeometryPatch::terminate()
{
	/* For cleanup, unschedule callback and stop the scheduler. */
	hdStopScheduler();
	if(hHapticEffect != NULL)
		hdUnschedule(hHapticEffect);

	/* Disable the device. */
	hdDisableDevice(hHD);

	return true;
}

HDdouble BaseGeometryPatch::getDevicePos(int index){ 
	if(index < 0) index = 0;
	if(index > 3) index = 3;
	return devicePos[index];
}

HDdouble BaseGeometryPatch::getDeviceAngle(int index){ 
	if(index < 0) index = 0;
	if(index > 3) index = 3;
	return deviceAngle[index];
}

HDdouble BaseGeometryPatch::getTransformMatrix(int index){ 
	if(index < 0) index = 0;
	if(index > 16) index = 16;
	return transformMat[index];
}

void BaseGeometryPatch::setPatchStiffness(double stiffness){
	stiffnessK = stiffness;
}

void BaseGeometryPatch::setSharedMemoryPointers(PhantomToSim *pts, SimToPhantom *stp){
	phantomToSim = pts;
	simToPhantom = stp;
}

HDCallbackCode BaseGeometryPatch::patchCalc(){
	HDErrorInfo error;
	hduVector3Dd forceVec;
	hduVector3Dd targForceVec;
	hduVector3Dd loopForceVec;
	hduVector3Dd patchMinusDeviceVec;
	hduVector3Dd sensorVec;
	double sensorRadius = 1.0;

	if(simToPhantom == NULL){
		return HD_CALLBACK_CONTINUE;
	}

	HHD hHD = hdGetCurrentDevice();

	// reset all the variables that need to accumulate
	for(int sensi = 0; sensi < SimToPhantom::NUM_SENSORS; ++sensi){
		phantomToSim->forceMagnitude[sensi] = 0;
		phantomToSim->forceVec[sensi][0] = 0;
		phantomToSim->forceVec[sensi][1] = 0;
		phantomToSim->forceVec[sensi][2] = 0;
	}


	// Begin haptics frame.  ( In general, all state-related haptics calls
	//	should be made within a frame. ) 
	hdBeginFrame(hHD);

	/* Get the current devicePos of the device. */
	hdGetDoublev(HD_CURRENT_POSITION, devicePos);
	hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, deviceAngle);
	hdGetDoublev(HD_CURRENT_TRANSFORM, transformMat);
	
	forceVec[0] = 0;
	forceVec[1] = 0;
	forceVec[2] = 0;

	
	if(simToPhantom->numTargets > SimToPhantom::MAX_TARGETS)
		simToPhantom->numTargets = SimToPhantom::MAX_TARGETS;

	// clear all the data structures
	if(phantomToSim != NULL){
		for(int i = 0; i < PhantomToSim::NUM_SENSORS; ++i){
			phantomToSim->forceMagnitude[i] = 0;
			phantomToSim->forceVec[i][0] = 0;
			phantomToSim->forceVec[i][1] = 0;
			phantomToSim->forceVec[i][2] = 0;
	
			for(int j = 0; j < PhantomToSim::MAX_TARGETS; ++j){
				phantomToSim->targForceMagnitude[j][i] = 0;
				phantomToSim->targForceVec[j][i][0] = 0;
				phantomToSim->targForceVec[j][i][1] = 0;
				phantomToSim->targForceVec[j][i][2] = 0;
			}
		}
		for(int j = 0; j < PhantomToSim::MAX_TARGETS; ++j){
			phantomToSim->targForcesMagnitude[j] = 0;
		}
	}

	/***/
	
	//printf("BaseGeometryPatch::patchCalc() numtargets = %d\n", simToPhantom->numTargets);
	for(int targi = 0; targi < simToPhantom->numTargets; ++targi){
		patchPos[0] = simToPhantom->targetX[targi];
		patchPos[1] = simToPhantom->targetY[targi];
		patchPos[2] = simToPhantom->targetZ[targi];
		patchRadius = simToPhantom->targetRadius[targi];

		targForceVec[0] = 0;
		targForceVec[1] = 0;
		targForceVec[2] = 0;
		for(int sensi = 0; sensi < SimToPhantom::NUM_SENSORS; ++sensi){
			loopForceVec[0] = 0;
			loopForceVec[1] = 0;
			loopForceVec[2] = 0;

			sensorVec[0] = simToPhantom->sensorX[sensi] + devicePos[0];
			sensorVec[1] = simToPhantom->sensorY[sensi] + devicePos[1];
			sensorVec[2] = simToPhantom->sensorZ[sensi] + devicePos[2];
			sensorRadius = simToPhantom->sensorRadius[sensi];


			//  patchMinusDeviceVec = patchPos-devicePos  < 
			//	Create a vector from the device devicePos towards the sphere's center. 
			//hduVecSubtract(patchMinusDeviceVec, patchPos, devicePos);
			hduVecSubtract(patchMinusDeviceVec, patchPos, sensorVec);
			hduVector3Dd dirVec;
			hduVecNormalize(dirVec, patchMinusDeviceVec);

	
			//  If the device position is within the sphere's surface
			//  center, apply a spring forceVec towards the surface.  The forceVec
			//	calculation differs from a traditional gravitational body in that the
			//	closer the device is to the center, the less forceVec the well exerts;
			//	the device behaves as if a spring were connected between itself and
			//	the well's center. *
			double penetrationDist = patchMinusDeviceVec.magnitude() - (patchRadius+sensorRadius);
			if(penetrationDist < 0)
			{
				// >  F = k * x  < 
				//	F: forceVec in Newtons (N)
				//	k: Stiffness of the well (N/mm)
				//	x: Vector from the device endpoint devicePos to the center 
				//	of the well. 
				hduVecScale(dirVec, dirVec, penetrationDist);
				hduVecScale(loopForceVec, dirVec, stiffnessK);
				/***
				for(int i = 0; i < 3; ++i)
					if(loopForceVec[i] < 0)
						loopForceVec[i] *= 0.5;
				printf("targForceVec[%d][%d] = %.2f, %.2f, %.2f\r", targi, sensi, loopForceVec[0], loopForceVec[1], loopForceVec[2]);
				/****/
			}

			if(phantomToSim != NULL){
				phantomToSim->forceMagnitude[sensi] += hduVecMagnitude(loopForceVec);
				phantomToSim->forceVec[sensi][0] += loopForceVec[0];
				phantomToSim->forceVec[sensi][1] += loopForceVec[1];
				phantomToSim->forceVec[sensi][2] += loopForceVec[2];

				phantomToSim->targForceMagnitude[targi][sensi] = hduVecMagnitude(loopForceVec);
				phantomToSim->targForceVec[targi][sensi][0] = loopForceVec[0];
				phantomToSim->targForceVec[targi][sensi][1] = loopForceVec[1];
				phantomToSim->targForceVec[targi][sensi][2] = loopForceVec[2];

					
			}

			hduVecAdd(targForceVec, targForceVec, loopForceVec);
			hduVecAdd(forceVec, forceVec, loopForceVec);

				
		}
		if(phantomToSim != NULL){
			phantomToSim->targForcesMagnitude[targi] = hduVecMagnitude(targForceVec);
		}
	}

	if(phantomToSim != NULL){
		for(int i = 0; i < 16; ++i){
			phantomToSim->matrix[i] = transformMat[i];
		}
	}
	
	// divide the forceVec the number of times that a force was added?

	/* Send the forceVec to the device. */
	if(simToPhantom->enabled){
		hdSetDoublev(HD_CURRENT_FORCE, forceVec);
	}
	
	/* End haptics frame. */
	hdEndFrame(hHD);



	/* Check for errors and abort the callback if a scheduler error
	   is detected. */
	if (HD_DEVICE_ERROR(error = hdGetError()))
	{
		hduPrintError(stderr, &error, "BaseGeometryPatch.calcPatch():\n");
		
		if (hduIsSchedulerError(&error))
		{
			return HD_CALLBACK_DONE;
		}
	}



	/* Signify that the callback should continue running, i.e. that
	   it will be called again the next scheduler tick. */
	return HD_CALLBACK_CONTINUE;
}

HDCallbackCode HDCALLBACK BaseGeometryPatch::patchCallback(void *data)
{
	BaseGeometryPatch *bgp = (BaseGeometryPatch *)data;
	return bgp->patchCalc();
} 