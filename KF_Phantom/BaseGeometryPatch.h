#pragma once

#include <HD/hd.h>
#include <HDU/hduError.h>
#include <HDU/hduVector.h>

#include "PhantomSimInterfaces.h"


class BaseGeometryPatch
{
public:
	BaseGeometryPatch(void);
	~BaseGeometryPatch(void);

	bool setup();
	bool run();
	bool terminate();
	HDCallbackCode patchCalc(); // overload this for different behaviors
	void setPatchStiffness(double stiffness);
	void setSharedMemoryPointers(PhantomToSim *pts, SimToPhantom *stp);
	HDdouble getDevicePos(int index);
	HDdouble getDeviceAngle(int index);
	HDdouble getTransformMatrix(int index);

protected: 
	HDErrorInfo error;
	HDSchedulerHandle hHapticEffect;
	HHD hHD;

	PhantomToSim *phantomToSim;
	SimToPhantom *simToPhantom;

	hduVector3Dd patchPos;
	hduVector3Dd patchVec;
	double patchRadius;
	double stiffnessK;
	hduVector3Dd devicePos;
	hduVector3Dd deviceAngle;
	HDdouble transformMat[16];

	static HDCallbackCode HDCALLBACK patchCallback(void *data);
};

