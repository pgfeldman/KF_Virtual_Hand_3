// KF_Phantom.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


#include "BaseGeometryPatch.h"
#include "SharedMemoryManager.h"
#include "PhantomSimInterfaces.h"

#include <math.h>

int _tmain(int argc, _TCHAR* argv[])
{
	SharedMemoryManager publisher;
	SharedMemoryManager subscriber;
	boolean result;
	PhantomToSim pts;
	SimToPhantom stp;

	// initialize the outgoing data structure

	for(int i = 0; i < pts.NUM_SENSORS; ++i){
		pts.forceMagnitude[i] = 0;
		pts.forceVec[0][i] = 0;
		pts.forceVec[1][i] = 0;
		pts.forceVec[2][i] = 0;
	}
	for(int i = 0; i < 16; ++i){
		pts.matrix[i] = 0;
	}
	pts.message[0] = NULL;

	unsigned int ptsSize = sizeof(pts);
	unsigned int stpSize = sizeof(stp);

	result = subscriber.createFile("KF_Sim_publish", stpSize);
	if(!result){
		printf("unable to open subscriber\n");
		getchar();
		return false;
	}
	
	result = publisher.createFile("KF_Phantom_publish", ptsSize);
	if(!result){
		printf("unable to open publisher\n");
		getchar();
		return false;
	}

	/****/
	BaseGeometryPatch bgp;
	bgp.setSharedMemoryPointers(&pts, &stp);

	
	if(!bgp.setup()){
		fprintf(stderr, "setup failed\n");
		fprintf(stderr, "Press CR to quit.\n\n");
		getchar();
		return -1;
	}

	if(!bgp.run()){
		fprintf(stderr, "run failed\n");
		fprintf(stderr, "Press CR to quit.\n\n");
		getchar();
		return -1;
	}


	fprintf(stderr, "Feel around for the haptic effect...\n");
	fprintf(stderr, "Hit any key to quit.\n\n");
	int c = 'c';
	while(!kbhit()){
		bgp.setPatchStiffness(0.1);

		sprintf(pts.message, "Phantom pos:(%.2f, %.2f, %.2f) force(%.2f, %.2f, %.2f)", 
			pts.matrix[12], pts.matrix[13], pts.matrix[14],
			pts.forceMagnitude[0], pts.forceMagnitude[1], pts.forceMagnitude[2]);
		
		publisher.copyToMemory(&pts, ptsSize);
		subscriber.copyFromMemory(&stp, stpSize);
		printf("%s, %s\r", pts.message, stp.message);

		Sleep(10);
	}


	bgp.terminate();
	/*****/
	fprintf(stderr, "\nHit CR to quit.\n\n");
	getchar();
	return 0;
}


