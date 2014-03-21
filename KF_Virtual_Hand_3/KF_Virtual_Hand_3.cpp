// KF_Virtual_Hand_3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"         
#include "KF_Virtual_Hand_3.h"

KF_Virtual_Hand_3::KF_Virtual_Hand_3(void){
	ba.init();
}

KF_Virtual_Hand_3::~KF_Virtual_Hand_3(void){
}

void KF_Virtual_Hand_3::createTarget(LPCWSTR result){
	LPCWSTR label = wcsrchr(result, '\\');
	if(label == NULL)
		label = wcsrchr(result, '/');
	label++;
	if(label == NULL)
		label = result;
	
	;
	float size = (float)frand(testManager.sizemin, testManager.sizemax);
	float x = (float)frand(testManager.xmin, testManager.xmax);
	float y = (float)frand(testManager.ymin, testManager.ymax);
	float z = (float)frand(testManager.zmin, testManager.zmax);

	float r = (float)frand(0.2, 1.0);
	float g = (float)frand(0.2, 1.0);
	float b = (float)frand(0.2, 1.0);
	TargetSphere *ts = kfHandWindow->addTarget(size, x, y, z, r, g, b);
	WavSampleSound *redSound = ts->addSound(&ba, Gripper::RED, label, result, XAUDIO2_LOOP_INFINITE);
	WavSampleSound *greenSound = ts->addSound(&ba, Gripper::GREEN, label, result, XAUDIO2_LOOP_INFINITE);
	redSound->start(); 
	greenSound->start(); 
	ba.playOnChannelVoice(redSound->getSourceVoice(), Gripper::RED, 0.0);
	ba.playOnChannelVoice(greenSound->getSourceVoice(), Gripper::GREEN, 0.0);

	hcui->soundChoice->add(redSound->getCName().c_str());
}

void KF_Virtual_Hand_3::controlSysCallback(void* data){
	KF_Virtual_Hand_3 *kvh = (KF_Virtual_Hand_3*)(data);
	HandControlUI	*gui = kvh->hcui;
	KF_Hand_Window	*kfhw = kvh->kfHandWindow;
	TestManager *tm = &(kvh->testManager);
	BasicAudio *ba = &(kvh->ba);
	//printf("KF_Virtual_Hand_3::controlSysCallback()\n");
	
	if(tm->curState == TestManager::STATE_RUNNING){
		gui->greenSensitivity->value(kfhw->getGripper()->getGripperForce(Gripper::GREEN));
		gui->redSensitivity->value(kfhw->getGripper()->getGripperForce(Gripper::RED));
	}

	UI_cmd* generalCmd = kvh->getCmd("general");
	if(generalCmd != NULL && generalCmd->isWaiting()){
		if(generalCmd->equals("singleSoundChoice")){
			fileChooser *fc = new fileChooser(".", "*.wav", Fl_File_Chooser::SINGLE, "Choose Single Sound File");
			LPCWSTR result = fc->show();
			wprintf(L"result = %s\n", result);
			if(result){
				kvh->createTarget(result);
			}
			delete fc;
			fc = NULL;
		} else if(generalCmd->equals("saveSetup")){
			fileChooser *fc = new fileChooser(".", "*.xml", Fl_File_Chooser::CREATE, "Choose Setup File");
			LPCWSTR result = fc->show();
			wprintf(L"result = %s\n", result);
			if(result){
				const char* warning = tm->setup(gui);
				if(warning == NULL){
					tm->writeSetupFile(result, kfhw);
				}else{
					fl_alert(warning);
				}
			}
			delete fc;
			fc = NULL;
		} else if(generalCmd->equals("openFile")){
			fileChooser *fc = new fileChooser(".", "*.xml", Fl_File_Chooser::SINGLE, "Choose Setup File");
			LPCWSTR result = fc->show();
			wprintf(L"result = %s\n", result);

			if(result){
				tm->loadSetupFile(result, gui);
				int seed = (int)(gui->seedInput->value());
				srand(seed);
				vector<string> vs = tm->getSoundFilenameVector();
				for(int i = 0; i < vs.size(); ++i){
					string str = vs[i];
					printf("filename[%d] = %s\n", i, str.c_str());
					wstring wstr(str.begin(), str.end());
					kvh->createTarget(wstr.c_str());
				}
			}

			delete fc;
			fc = NULL;
		} else if(generalCmd->equals("close")){
			if(fl_ask("Are you sure you want to exit?"))
					gui->mainWindow->hide();

			if(!tm->safeToExit()){
				if(fl_ask("Are you sure you want to exit? Test is incomplete"))
					gui->mainWindow->hide();
			}else{
				gui->mainWindow->hide();
			}

		}else if(generalCmd->equals("start")){
			// configure the test manager class. If it's incomplete, then show an alert, saying what needs to be done
			printf("Test Starting\n");
			const char* warning = tm->setup(gui);
			kfhw->getGoalBox()->setScalar(tm->goalboxsize);
			kfhw->setEnvDimensions(tm->xmin, tm->xmax, tm->ymin, tm->ymax, tm->zmin, tm->zmax);

			// start the test
			if(!warning){
				//tm->curState = TestManager::STATE_READY;
				int result = tm->getTestState();
				if(result == -1){
					fl_alert("There are no test modes to increment through");
					tm->curState = TestManager::STATE_INCOMPLETE;
				}else{
					tm->curState = TestManager::STATE_RUNNING;
					kvh->incrementTest(result); 
					gui->nextButton->activate();
					gui->startButton->deactivate();
				}		  
			}else{		  
				fl_alert(warning);
				tm->curState = TestManager::STATE_INCOMPLETE;
			}
		}else if(generalCmd->equals("next")){
			printf("Next\n");
			tm->finishAttempt(kfhw, gui, ba); 
			tm->goalCount = 0;
			int result = tm->getTestState();

			if(result == TestManager::TEST_SESSION_COMPLETE){ 
				printf("session complete\n");
				fileChooser *fc = new fileChooser(".", "*.csv", Fl_File_Chooser::CREATE, "Choose Output File");
				LPCWSTR result = fc->show();
				wprintf(L"result = %s\n", result);
				if(result){
					tm->writeFile(result, kfhw);
				}
				delete fc;
				fc = NULL;
				// write out file, reset things
				tm->curState = TestManager::STATE_INCOMPLETE;
				tm->setup(gui);

				fl_alert("To maintain separation of data between attempts, this application will now close. Please re-launch for a new test.");
				gui->mainWindow->hide();
			}else{
				// go to the next 
				tm->startAttempt();
				kvh->incrementTest(result);
			}
		}else if(generalCmd->equals("taskComplete")){ // record and then wait for the user to click the "next" command
			printf("taskComplete\n");
			gui->testDirections->value("Please hit 'Next' to continue");
			gui->nextButton->activate();
			tm->finishAttempt(kfhw, gui, ba); 
			// save type and timing for this result
		}
		else if(generalCmd->equals("env")){ // record and then wait for the user to click the "next" command
			printf("env\n");
			tm->xmin = (float)gui->xmin->value();
			tm->xmax = (float)gui->xmax->value();
			tm->ymin = (float)gui->ymin->value();
			tm->ymax = (float)gui->ymax->value();
			tm->zmin = (float)gui->zmin->value();
			tm->zmax = (float)gui->zmax->value();

			tm->sizemin = (float)gui->targetSphereMin->value();
			tm->sizemax = (float)gui->targetSphereMax->value();

			tm->goalboxsize = (float)gui->goalBoxSize->value();

			kfhw->getGoalBox()->setScalar((float)gui->goalBoxSize->value());
			kfhw->setEnvDimensions(tm->xmin, tm->xmax, tm->ymin, tm->ymax, tm->zmin, tm->zmax);
			kfhw->constrainTargets((bool)gui->constrainTargetsCheck->value());
		}else if(generalCmd->equals("testType")){
			const char* label = gui->testTypeCombo->text();
			gui->testDirections->value(label);
			if(strcmp(label, "No Feedback") == 0){
				kfhw->enableHaptics(false);
				kfhw->enableAudio(false);
				tm->curTest = TestManager::OPEN_LOOP_TEST;
			}else if(strcmp(label, "Haptic") == 0){
				kfhw->enableHaptics(true);
				kfhw->enableAudio(false);
				tm->curTest = TestManager::HAPTIC_TEST;
			}else if(strcmp(label, "Tactor") == 0){
				kfhw->enableHaptics(false);
				kfhw->enableAudio(true);
				tm->curTest = TestManager::TACTOR_TEST;
			}else if(strcmp(label, "Haptic and Tactor") == 0){
				kfhw->enableHaptics(true);
				kfhw->enableAudio(true);
				tm->curTest = TestManager::HAPTIC_TACTOR_TEST;
			}else{
				fl_alert("%s choice not supported", label);
			}
		}else if(generalCmd->equals("practice")){
			srand((unsigned int)clock());
			tm->resetTargets(kfhw);
		}
		generalCmd->clear();
	}

	UI_cmd *simCmd = kvh->getCmd("simulation");
	if(simCmd != NULL && simCmd->isWaiting()){

		if(simCmd->equals("GoalBox:inside")){
			tm->goalCount++;
			TargetSphere *ts = (TargetSphere*)(simCmd->getData());
			ts->setPostion(0, 400, 0); // get the sphere out of the way
			printf("deleted TargetSphere '%s'. %d of %d remaining\n", ts->getSoundPtr(0)->getCName().c_str(), tm->goalCount, kfhw->getNumTargets());
			
			if(tm->goalCount >= kfhw->getNumTargets()){
				if(tm->curState == TestManager::STATE_RUNNING)
					generalCmd->set("taskComplete", NULL);
				else
					generalCmd->set("practice", NULL);
			}
		}
		simCmd->clear();
	}
	Fl::repeat_timeout(kvh->refreshSeconds, controlSysCallback, data);
}

void KF_Virtual_Hand_3::incrementTest(int result){
	testManager.startAttempt();
	testManager.curTest = result;
	switch(result){
		case TestManager::OPEN_LOOP_TEST:
			kfHandWindow->enableHaptics(false);
			kfHandWindow->enableAudio(false);
			testManager.incrementOpenLoopTest(kfHandWindow, hcui);
			break;
		case TestManager::HAPTIC_TEST:
			kfHandWindow->enableHaptics(true);
			kfHandWindow->enableAudio(false);
			testManager.incrementHapticTest(kfHandWindow, hcui);
			break;
		case TestManager::TACTOR_TEST:
			kfHandWindow->enableHaptics(false);
			kfHandWindow->enableAudio(true);
			testManager.incrementTactorTest(kfHandWindow, hcui);
			break;
		case TestManager::HAPTIC_TACTOR_TEST:
			kfHandWindow->enableHaptics(true);
			kfHandWindow->enableAudio(true);
			testManager.incrementHapticTactorTest(kfHandWindow, hcui);
			break;
	}	  				
}

void KF_Virtual_Hand_3::init(int argc, _TCHAR* argv[]){
	refreshSeconds = 0.1f;

	//cmd.setName("general");
	UI_cmd *cmd = createCommand("general");

	hcui = new HandControlUI();
	hcui->setCmdPtr(cmd);

	Fl::visual(FL_DOUBLE|FL_INDEX);
	hcui->show(argc, (char**)argv);

	cmd = createCommand("simulation");
	kfHandWindow = hcui->handView;
	kfHandWindow->setBasicAudioPtr(&ba);
	kfHandWindow->setCmdPointer(cmd);

	hcui->nextButton->deactivate();
	
	Fl::add_timeout(refreshSeconds, controlSysCallback, this);
}

void KF_Virtual_Hand_3::run(){
	Fl::run();
}

void KF_Virtual_Hand_3::terminate(){
	ba.destroy();
}

int _tmain(int argc, _TCHAR* argv[])
{
	KF_Virtual_Hand_3 *kvh = new KF_Virtual_Hand_3();
	kvh->init(argc, argv);
	kvh->run();

	return 0;
}

