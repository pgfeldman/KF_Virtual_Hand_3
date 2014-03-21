#pragma once

#define _MAX_TARGETS 10
#define _MESSAGE_CHARS 255
#define _NUM_SENSORS 8

struct PhantomToSim{
	static const int MAX_TARGETS = _MAX_TARGETS;
	static const int MESSAGE_CHARS = _MESSAGE_CHARS;
	static const int NUM_SENSORS = _NUM_SENSORS;

	double forceMagnitude[NUM_SENSORS];
	double forceVec[NUM_SENSORS][3];
	double matrix[16];
	double targForcesMagnitude[MAX_TARGETS];
	double targForceMagnitude[MAX_TARGETS][NUM_SENSORS];
	double targForceVec[MAX_TARGETS][NUM_SENSORS][3];
	char message[MESSAGE_CHARS];
};

struct SimToPhantom{
	static const int MAX_TARGETS = _MAX_TARGETS;
	static const int MESSAGE_CHARS = _MESSAGE_CHARS;
	static const int NUM_SENSORS = _NUM_SENSORS;

	int numTargets;
	int enabled;
	double targetX[MAX_TARGETS];
	double targetY[MAX_TARGETS];
	double targetZ[MAX_TARGETS];
	double targetRadius[MAX_TARGETS];
	bool targetActive[MAX_TARGETS];
	double sensorX[NUM_SENSORS];
	double sensorY[NUM_SENSORS];
	double sensorZ[NUM_SENSORS];
	double sensorRadius[NUM_SENSORS];
	char message[MESSAGE_CHARS];
};