#include "StdAfx.h"
#include "KF_Hand_Window.h"


KF_Hand_Window::KF_Hand_Window(int x,int y,int w,int h,const char *l)
	: Gl_ShaderWindow(x,y,w,h,l)
{
	setWorldPos(0, -2, -500);

	curTime = clock();
	prevTime = clock();
	deltaTime = 0.0f;
	elapsed = 0.0f;
	mouseWheelScalar = 10.0f;

	reverseMouseX = false;
	reverseMouseY = false;

	ba = NULL;
	audioEnabled = true;

	result = subscriber.createFile("KF_Phantom_publish",255);
	if(!result){
		fl_alert("KF_Hand_Window.ctor: unable to open subscriber\n");
		printf("KF_Hand_Window.ctor: unable to open subscriber\n");
	}

	result = publisher.createFile("KF_Sim_publish",255);
	if(!result){
		fl_alert("KF_Hand_Window.ctor: unable to open publisher\n");
		printf("KF_Hand_Window.ctor: unable to open publisher\n");
	}
	
	simToPhantom.numTargets = 0;
	for(int i = 0; i < simToPhantom.numTargets; ++i){
		simToPhantom.targetX[i] = 0.0;
		simToPhantom.targetY[i] = 0.0;
		simToPhantom.targetZ[i] = 0.0;
		simToPhantom.targetRadius[i] = 5.0;
		simToPhantom.targetActive[i] = true;
	}
	simToPhantom.sensorX[0] = 0.0;
	simToPhantom.sensorY[0] = 0.0;
	simToPhantom.sensorZ[0] = 0.0;
	simToPhantom.sensorRadius[0] = 1.0;
	simToPhantom.enabled = true;
	sprintf(simToPhantom.message, "Sim numTargets = %d", simToPhantom.numTargets);

	publisher.copyToMemory(&simToPhantom, sizeof(simToPhantom));

	srand(1);

	xmin = -200;
	xmax = 200;
	ymin = -100;
	ymax = 150;
	zmin = -100;
	zmax = 10;
}


KF_Hand_Window::~KF_Hand_Window(void)
{
}

void KF_Hand_Window::localInit(){
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	gridStage = new GridStage(200.0f, 5);

	gripper = new Gripper();
	gripper->setScalar(10.0);
	gripper->setColor(1.0, 1.0, 0.0, 1.0);
	gripper->setEffectorScalar(Gripper::RED, 0.23);
	gripper->setEffectorScalar(Gripper::GREEN, 0.23);

	goalBox = new GoalBox();
	goalBox->setScalar(30.0);
	goalBox->setColor(1.0, 1.0, 1.0, 1.0);
	goalBox->setPostion(0, -100, -100);
}

void KF_Hand_Window::resize(){

}

void KF_Hand_Window::stepControlSys(){
	calcDeltaTime();
}

void KF_Hand_Window::environmentCalc(){

	// get data from the input devices and set the graphics objects
	subscriber.copyFromMemory(&phantomToSim, sizeof(phantomToSim));

	
	double *mat = phantomToSim.matrix;
	/****
	for(int i = 0; i < 16; i += 4){
		Dprint::add("row[%d] = %.2f, %.2f, %.2f, %.2f", i/4, mat[i], mat[i+1], mat[i+2], mat[i+3]);
	}/****/

	float xpos = (float)mat[12];
	float ypos = (float)mat[13];
	float zpos = (float)mat[14];

	gripper->setPostion(xpos, ypos, zpos);
	
	//Dprint::add("Gripper - Red force %.2f, Green force %.2f)", 
	//	phantomToSim.forceMagnitude[Gripper::RED], phantomToSim.forceMagnitude[Gripper::GREEN]);

	for(int i = 0; i < PhantomToSim::NUM_SENSORS; ++i)
		gripper->setContactForce(i, phantomToSim.forceMagnitude[i]);

	float fmat[16];
	for(int i = 0; i < 16; ++i){
		fmat[i] = (float)(phantomToSim.matrix[i]);
	}
	gripper->environmentCalc(modelViewMatrix, fmat);

	float *offsets[PhantomToSim::NUM_SENSORS];
	for(int i = 0; i < PhantomToSim::NUM_SENSORS; ++i){
		offsets[i] = gripper->getGripperOffest(i);
	}
	
	double posVecs[PhantomToSim::NUM_SENSORS][3];
	for(int i = 0; i < PhantomToSim::NUM_SENSORS; ++i){
		for(int j = 0; j < 3; ++j)
			posVecs[i][j] = gripper->getGripperPosition(i)[j];
	}

	TargetSphere *ts;
	for(int i = 0; i < simToPhantom.numTargets; ++i){
		ts = targetSphereArray[i];
		
		if(ts->isEnabled()){
			for(int j = 0; j < PhantomToSim::NUM_SENSORS; ++j)
				ts->setSensorPos(i, posVecs[j]);
			ts->environmentCalc();

			if(targetsConstrained){
				float tx = ts->getXpos();
				float ty = ts->getYpos();
				float tz = ts->getZpos();
				tx = max(xmin, tx);
				tx = min(xmax, tx);
				ty = max(ymin, ty);
				ty = min(ymax, ty);
				tz = max(zmin, tz);
				tz = min(ymax, tz);
				ts->setPostion(tx, ty, tz);
				Dprint::add("constrining target [%d] to (%.2f, %.2f, %.2f)", i, tx, ty, tz);
			}
		
			if(goalBox->isTargetInside(ts))
			{
				cmd->set("GoalBox:inside", ts);
				ts->setEnabled(false);
			}
		}
		//Dprint::add("Target[%d] touching = %d, inside = %d", i, goalBox->isTargetTouching(ts), goalBox->isTargetInside(ts));
	}
	
	//Dprint::add("model: (%.2f, %.2f)", modelPos[0], modelPos[2]);

	float modelPosX = modelPos[0];
	if(reverseMouseX)
		modelPosX *= -1.0;
	float modelPosY = modelPos[2];
	if(reverseMouseY)
		modelPosY *= -1.0;

	stepControlSys();

	// calculate the sound volume for each target.
	if(audioEnabled && (ba != NULL)){
		double r[4];
		double g[4];
		
		for(int t = 0; t < simToPhantom.numTargets; ++t){
			ts = targetSphereArray[t];
			WavSampleSound *redSound = ts->getSoundPtr(Gripper::RED);
			WavSampleSound *greenSound = ts->getSoundPtr(Gripper::GREEN);
			if((redSound != NULL) && (greenSound != NULL)){
				r[0] = phantomToSim.targForceMagnitude[t][Gripper::RED];
				r[1] = phantomToSim.targForceMagnitude[t][Gripper::RED2];
				r[2] = phantomToSim.targForceMagnitude[t][Gripper::RED3];
				r[3] = phantomToSim.targForceMagnitude[t][Gripper::RED4];
				g[0] = phantomToSim.targForceMagnitude[t][Gripper::GREEN];
				g[1] = phantomToSim.targForceMagnitude[t][Gripper::GREEN2];
				g[2] = phantomToSim.targForceMagnitude[t][Gripper::GREEN3];
				g[3] = phantomToSim.targForceMagnitude[t][Gripper::GREEN4];
				double rvol = r[0];
				double gvol = g[0];
				for(int i = 1; i < 4; ++i){
					rvol = max(rvol, r[i]);
					gvol = max(gvol, g[i]);
				}
				ba->playOnChannelVoice(greenSound->getSourceVoice(), Gripper::GREEN, (float)min(1.0, gvol));
				ba->playOnChannelVoice(redSound->getSourceVoice(), Gripper::RED, (float)min(1.0, rvol));
				//Dprint::add("Target[%d] - Red force %.2f, Green force %.2f)", t, r, g);
			}
		}
	}

	for(int i = 0; i < simToPhantom.numTargets; ++i){
		ts = targetSphereArray[i];
		simToPhantom.targetX[i] = ts->getXpos();
		simToPhantom.targetY[i] = ts->getYpos();
		simToPhantom.targetZ[i] = ts->getZpos();
		simToPhantom.targetRadius[i] = ts->getScalar();
	}
	for(int i = 0; i < PhantomToSim::NUM_SENSORS; ++i){
		simToPhantom.sensorX[i] = offsets[i][0];
		simToPhantom.sensorY[i] = offsets[i][1];
		simToPhantom.sensorZ[i] = offsets[i][2];
		simToPhantom.sensorRadius[i] = gripper->getScalar();
	}

	/****
	for(int i = 0; i < PhantomToSim::NUM_SENSORS; ++i){
		Dprint::add("sensor[%d]: (%.2f, %.2f, %.2f)", i, simToPhantom.sensorX[i], simToPhantom.sensorY[i], simToPhantom.sensorZ[i]);
	}
	/****/


	publisher.copyToMemory(&simToPhantom, sizeof(simToPhantom));
	
}

void KF_Hand_Window::draw(){
	DrawableObject::PICK_RESULT pickResult;

	
		// 3D draw stuff here!

	if(isPicking){
		preDraw3D();
			gridStage->pickRender(modelViewMatrix, projectionMatrix, shaderManager);
			pickResult = gridStage->pickResult();
			if(pickResult == DrawableObject::HIT)
				printf("Gridstage Hit\n");
			else if(pickResult == DrawableObject::MISS)
				printf("Gridstage Miss\n");
			else if(pickResult == DrawableObject::UNAVAILABLE)
				printf("Gridstage Unavailable\n");
		postDraw3D();
	}

	preDraw3D();

		gridStage->render(modelViewMatrix, projectionMatrix, shaderManager);

		float mat[16];
		for(int i = 0; i < 16; ++i){
			mat[i] = (float)(phantomToSim.matrix[i]);
		}
		gripper->render(modelViewMatrix, projectionMatrix, mat, shaderManager);

		TargetSphere *ts;
		for(int i = 0; i < simToPhantom.numTargets; ++i){
			ts = targetSphereArray[i];
			ts->render(modelViewMatrix, projectionMatrix, shaderManager);
		}

		goalBox->render(modelViewMatrix, projectionMatrix, shaderManager);

		/***
		cCubeStone->render(modelViewMatrix, projectionMatrix, shaderManager);
		cCubeCarpet->render(modelViewMatrix, projectionMatrix, shaderManager);
		****/
		

	postDraw3D();

	
	draw2D();
}

void KF_Hand_Window::localCleanup(){
	gridStage->cleanup();
	delete(gridStage);

	gripper->cleanup();
	delete(gripper);

	TargetSphere *ts;
	for(int i = 0; i < simToPhantom.numTargets; ++i){
		ts = targetSphereArray[i];
		ts->cleanup();
		delete(ts);
	}

	/***
	cCubeStone->cleanup();
	delete(cCubeStone);

	cCubeCarpet->cleanup();
	delete(cCubeCarpet);
	****/

	
}