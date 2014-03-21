#pragma once

#include "stdafx.h"
#include <GLTools.h>            // OpenGL toolkit
#include <FL/Fl.H>
#include <FL/Fl_Output.H>

#include "config.h"
#include "Gl_ShaderWindow.h"
#include "GridStage.h"
#include "TargetSphere.h"
#include "Gripper.h"
#include "GoalBox.h"


#include "CollisionCube.h"
#include "TexturedCollisionCube.h"

#include "UI_cmd.h"

#include "WavSampleSound.h"
#include "BasicAudio.h"

#include "SharedMemoryManager.h"
#include "PhantomSimInterfaces.h"


class KF_Hand_Window :
	public Gl_ShaderWindow

{
public:
	KF_Hand_Window(int x,int y,int w,int h,const char *l);
	~KF_Hand_Window(void);
	void environmentCalc();
	void draw();
	void localInit();
	void resize();
	virtual void localCleanup();

	void setBasicAudioPtr(BasicAudio *baPtr){ba = baPtr;};

	void setCmdPointer(UI_cmd *ptr){cmd = ptr;};

	void constrainTargets(bool val){
		targetsConstrained = val;
		printf("KF_Hand_Window.constrainTargets = %d\n", targetsConstrained);
	};

	void setEnvDimensions(float xmn, float xmx, float ymn, float ymx, float zmn, float zmx){
		xmin = xmn;
		xmax = xmx;
		ymin = ymn;
		ymax = ymx;
		zmin = zmn;
		zmax = zmx;
	};

	TargetSphere *addTarget(float size, float x, float y, float z, float r, float g, float b){
		printf("New Sphere: pos(%.2f, %.2f, %.2f), color: (%.2f, %.2f, %.2f)\n", x, y, z, r, g, b);
		int index = simToPhantom.numTargets;
		if(index < PhantomToSim::MAX_TARGETS){
			TargetSphere *ts = new TargetSphere();
			ts->setScalar(size);
			ts->setPostion(x, y, z);
			ts->setColor(r, g, b, 1.0);
			ts->setPhantomPointers(&phantomToSim, &simToPhantom, index);
			targetSphereArray[index] = ts;
			simToPhantom.numTargets++;
			return ts;
		}
		return NULL;
	};

	void enableHaptics(bool val){simToPhantom.enabled = val;};
	void enableAudio(bool val){
		audioEnabled = val;
		if(audioEnabled){
			for(int i = 0; i < getNumTargets(); ++i){
				TargetSphere *ts = getTargetSphere(i);
				ts->getSoundPtr(0)->start();
				ts->getSoundPtr(1)->start();
			}
		}else{
			for(int i = 0; i < getNumTargets(); ++i){
				TargetSphere *ts = getTargetSphere(i);
				ts->getSoundPtr(0)->stop();
				ts->getSoundPtr(1)->stop();
			}
		}
	};

	int getNumTargets() {return simToPhantom.numTargets;};

	string getTargetInfo(int i){
		TargetSphere *ts = targetSphereArray[i];
		if(ts == NULL)
			return NULL;
		WavSampleSound *ws = ts->getSoundPtr(0);

		return ws->getCFileName();
	};

	TargetSphere *getTargetSphere(int i){
		i = min(i, simToPhantom.numTargets);
		i = max(i, 0);
		return  targetSphereArray[i];
	}
	
	bool reverseMouseX;
	bool reverseMouseY;
	
	// Access the TextOutput
	void SetTextOutput(Fl_Output* _textOutput)	{ 
		textOutput = _textOutput;
	};

	// Access the TextInput
	void SetTextInput(Fl_Output* _textInput)	{ 
		textInput = _textInput;
	};

	Gripper *getGripper(){return gripper;};
	GoalBox *getGoalBox(){return goalBox;};


	static const int MAX_ENTRIES = 32;
	static const int BUF_SIZE = 500;
	
private:
	/**
	 * @fn	calcDeltaTime()
	 *
	 * @brief	Calculates the delta time between the current and previous frame
	 *
	 * @author	Phil
	 * @date	3/14/2012
	 */
	void calcDeltaTime(){
		curTime = clock();
		deltaTime = (float)(curTime-prevTime)/CLOCKS_PER_SEC;
		prevTime = curTime;
		elapsed += deltaTime;
		Dprint::add("prev time = %d, cur time = %d, deltaTime = %f, elapsed = %f", prevTime, curTime, deltaTime, elapsed);
	}

	void stepControlSys();

	UI_cmd						*cmd;

	GridStage					*gridStage;
	Gripper						*gripper;
	TargetSphere				*targetSphereArray[SimToPhantom::MAX_TARGETS];
	GoalBox						*goalBox;
	bool						audioEnabled;


	Fl_Output					*textOutput;
	Fl_Output					*textInput;
	char						textBuffer[1024];
	char						inbuffer[BUF_SIZE];

	// shared memory
	SharedMemoryManager			subscriber;
	SharedMemoryManager			publisher;
	boolean						result;
	PhantomToSim				phantomToSim;
	SimToPhantom				simToPhantom;

	// environment range
	float	xmin;
	float	xmax;
	float	ymin;
	float	ymax;
	float	zmin;
	float	zmax;
	bool	targetsConstrained;
	/**
	 * @summary	The current time.
	 */
	clock_t curTime;

	/**
	 * @summary	Time of the previous frame.
	 */
	clock_t prevTime;

	/**
	 * @summary	delta time between frames
	 */
	float deltaTime;
	float elapsed;

	
	BasicAudio *ba;
};

