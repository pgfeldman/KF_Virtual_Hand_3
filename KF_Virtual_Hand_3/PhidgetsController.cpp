#include "StdAfx.h"
#include "PhidgetsController.h"
#include "Dprint.h"


PhidgetsController::PhidgetsController(void)
{
}


PhidgetsController::~PhidgetsController(void)
{
}

int CCONV CAttachHandler(CPhidgetHandle IFK, void *userptr)
{
	((PhidgetsController *)userptr)->AttachHandler(IFK);
	return 0;
}
int CCONV CDetachHandler(CPhidgetHandle IFK, void *userptr)
{
	((PhidgetsController *)userptr)->DetachHandler(IFK);
	return 0;
}

int CCONV CErrorHandler(CPhidgetHandle IFK, void *userptr, int ErrorCode, const char *unknown)
{
	((PhidgetsController *)userptr)->ErrorHandler(IFK,ErrorCode,unknown);	
	return 0;
}

void PhidgetsController::start(int milliseconds)
{
	_sampleTime = milliseconds;
	CPhidgetInterfaceKit_create(&_ifkit);
	CPhidget_set_OnAttach_Handler((CPhidgetHandle)_ifkit, CAttachHandler, this);
	CPhidget_set_OnDetach_Handler((CPhidgetHandle)_ifkit, CDetachHandler, this);
	CPhidget_set_OnError_Handler((CPhidgetHandle)_ifkit, CErrorHandler, this);
	CPhidget_open((CPhidgetHandle)_ifkit, -1);

	// set up the analog inputs
	CPhidgetInterfaceKit_setDataRate(_ifkit, 0, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 1, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 2, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 3, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 4, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 5, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 6, _sampleTime);
	CPhidgetInterfaceKit_setDataRate(_ifkit, 7, _sampleTime);	
}

void PhidgetsController::sampleAll(){
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 0, &_raw[0]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 1, &_raw[1]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 2, &_raw[2]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 3, &_raw[3]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 4, &_raw[4]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 5, &_raw[5]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 6, &_raw[6]);
	CPhidgetInterfaceKit_getSensorValue(_ifkit, 7, &_raw[7]);

	Dprint::add("raw: 0:[%d]\t1:[%d]\t2:[%d]\t3:[%d]\t4:[%d]\t5:[%d]\t6:[%d]\t7:[%d]", _raw[0], _raw[1], _raw[2], _raw[3], _raw[4], _raw[5], _raw[6], _raw[7]); 
}

int PhidgetsController::getRaw(int index){
	int safe = index;
	if(safe < 0) safe = 0;
	if(safe > 7) safe = 7;
	return _raw[safe];
}

void PhidgetsController::terminate(){
	CPhidget_close((CPhidgetHandle)_ifkit);
}

int PhidgetsController::AttachHandler(CPhidgetHandle IFK)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName(IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);

	printf("%s %10d attached!\n", name, serialNo);

	return 0;
}

int PhidgetsController::DetachHandler(CPhidgetHandle IFK)
{
	int serialNo;
	const char *name;

	CPhidget_getDeviceName (IFK, &name);
	CPhidget_getSerialNumber(IFK, &serialNo);

	printf("%s %10d detached!\n", name, serialNo);

	return 0;
}

int PhidgetsController::ErrorHandler(CPhidgetHandle IFK, int ErrorCode, const char *unknown)
{
	printf("Error handled. %d - %s", ErrorCode, unknown);

	return 0;
}