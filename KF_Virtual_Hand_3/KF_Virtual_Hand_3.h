#pragma once
#include <FL/Fl.H>

#include <vector>
#include <map>
#include "BasicAudio.h"
#include "UI_cmd.h"
#include "fileChooser.h"
#include "WavSampleSound.h"
#include "TestManager.h"


#include "handControl3_UI.h"
#include "KF_Hand_Window.h"

using namespace std;

#define UI_CMD_MAP map<string, UI_cmd>

class KF_Virtual_Hand_3
{
public:
	KF_Virtual_Hand_3(void);
	~KF_Virtual_Hand_3(void);

	UI_cmd *getCmd(char* name){
		UI_CMD_MAP::iterator it;
		
		string nameStr(name);
		it = cmdMap.find(nameStr);
		if(it != cmdMap.end())
			return &(it->second);

		return NULL;
	}

	UI_cmd *createCommand(char* name){
		UI_cmd cmd;
		string nameStr(name);
		cmd.setName(name);
		cmdMap[nameStr] = cmd;
		return getCmd(name);
	}

	void incrementTest(int result);

	void init(int argc, _TCHAR* argv[]);
	void run(void);
	void terminate();

	void createTarget(LPCWSTR result);

	static inline double frand(double fMin, double fMax)
	{
		double f = (double)rand() / RAND_MAX;
		return fMin + f * (fMax - fMin);
	}
	
protected:
	static void controlSysCallback(void* data);
	float			refreshSeconds;
	
	HandControlUI	*hcui;
	KF_Hand_Window	*kfHandWindow;
	//UI_cmd			cmd;
	UI_CMD_MAP		cmdMap;	

	BasicAudio		ba;
	TestManager		testManager;
};