#pragma once
#include <stdio.h>
#include <phidget21.h>

class PhidgetsController
{
public:
	PhidgetsController(void);
	~PhidgetsController(void);

	void start(int milliseconds = 10);
	void terminate();
	int AttachHandler(CPhidgetHandle IFK);
	int DetachHandler(CPhidgetHandle IFK);
	int ErrorHandler(CPhidgetHandle IFK, int ErrorCode, const char *unknown);
	void sampleAll();
	int getRaw(int index);
private:
	CPhidgetInterfaceKitHandle _ifkit;
	int _sampleTime; // milliseconds
	int _raw[8];
};

