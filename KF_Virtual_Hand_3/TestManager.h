#pragma once
#include <string>
#include <vector>
#include <ctime>
#include <D3DX10math.h>
#include "handControl3_UI.h"
#include "config.h"
#include "TestResult.h"
#include "KF_Hand_Window.h"
#include "WavSampleSound.h"
#include "BasicAudio.h"

using namespace std;
class TestManager
{
public:
	TestManager(void);
	~TestManager(void);
	const char* setup(HandControlUI *gui);
	bool incrementOpenLoopTest(KF_Hand_Window *glw, HandControlUI *gui);
	bool incrementHapticTest(KF_Hand_Window *glw, HandControlUI *gui);
	bool incrementTactorTest(KF_Hand_Window *glw, HandControlUI *gui);
	bool incrementHapticTactorTest(KF_Hand_Window *glw, HandControlUI *gui);
	bool incrementSession(HandControlUI *gui);
	void addResult(KF_Hand_Window *glw, BasicAudio *ba);
	void resetTargets(KF_Hand_Window *glw);
	void startAttempt();

	void finishAttempt(KF_Hand_Window *glw, HandControlUI *gui, BasicAudio *ba);

	void writeFile(LPCWSTR fptr, KF_Hand_Window *kfhw);
	void writeSetupFile(LPCWSTR fptr, KF_Hand_Window *kfhw);
	void loadSetupFile(LPCWSTR fptr, HandControlUI *gui);
	vector<string> getSoundFilenameVector(){ return soundFilenameVector;};

	boolean safeToExit(){
		if(curState == STATE_READY)
			return true;
		if(curState == STATE_COMPLETE)
			return true;
		if(curState == STATE_INCOMPLETE)
			return true;
		return false;
	};

	int getTestState();

	static inline double frand(double fMin, double fMax)
	{
		double f = (double)rand() / RAND_MAX;
		return fMin + f * (fMax - fMin);
	}

	static const int STATE_READY = 0;
	static const int STATE_RUNNING = 1;
	static const int STATE_COMPLETE = 2;
	static const int STATE_SESSION = 3;
	static const int STATE_INCOMPLETE = 4;

	static const int OPEN_LOOP_TEST = 0;
	static const int HAPTIC_TEST = 1;
	static const int TACTOR_TEST = 2;
	static const int HAPTIC_TACTOR_TEST = 3;
	static const int TEST_SESSION_COMPLETE = -1;

	char	researcherName[512];
	char	subjectName[512];
	char	subjectAge[512];
	char	subjectGender[512];
	char	testType[512];
	
	float	xmin;
	float	xmax;
	float	ymin;
	float	ymax;
	float	zmin;
	float	zmax;
	float	sizemin;
	float	sizemax;
	float	goalboxsize;
	int		seed;
	int		numOpenLoopTests;
	int		curOpenLoopTest;
	int		numHapticTests;
	int		curHapticTest;
	int		numTactorTests;
	int		curTactorTest;
	int		numHapticTactorTests;
	int		curHapticTactorTest;
	int		curTest;
	int		numSessions;
	int		curSession;
	int		curState;
	int		savedState;
	int		sphereCount;
	int		goalCount;
	bool	testRunning;
	clock_t  startTime;

private:
		char strbuf[1024];
		vector<TestResult*> resultVector;
		vector<string> soundFilenameVector;
};

