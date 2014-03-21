#pragma once
#include "simplesphere.h"
#include "PhantomSimInterfaces.h"
#include "WavSampleSound.h"
#include "BasicAudio.h"

class TargetSphere :
	public SimpleSphere
{
public:
	TargetSphere(void);
	~TargetSphere(void);

	void setPhantomPointers(PhantomToSim *pts, SimToPhantom *stp, int index){
		phantomToSim = pts;
		simToPhantom = stp;
		targIndex = index;
	}

	void setEnabled(bool val){enabled = val;};
	bool isEnabled(){return enabled;};

	void setSensorPos(int index, double pos[3]){
		for(int i = 0; i < 3; ++i){
			sensorPos[index][i] = pos[i];
		}
	}

	void setName(const char *nptr){strcpy_s(name, nptr);};

	WavSampleSound *addSound(BasicAudio *ba, int index, LPCWSTR soundName, LPCWSTR strFilename, UINT loopCount){
		continuousSound = (WavSampleSound *)ba->createSound(soundName, strFilename, loopCount);
		soundArray[index] = continuousSound;
		return continuousSound;
	};
	WavSampleSound *getSoundPtr(int index){
		index = min(index, MAX_SOUNDS);
		index = max(index, 0);
		return soundArray[index];
	};

	void render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager);
	void environmentCalc();

	static const int MAX_SOUNDS = 2;

private:
	double constraintAnchorPos[3];
	double velocityVec[3];
	double gripRatio;
	bool enabled;

	double sensorPos[SimToPhantom::NUM_SENSORS][3];
	double drag;
	char name[255];
	PhantomToSim *phantomToSim;
	SimToPhantom *simToPhantom;
	int targIndex;
	
	WavSampleSound *continuousSound;
	WavSampleSound* soundArray[MAX_SOUNDS];
};

