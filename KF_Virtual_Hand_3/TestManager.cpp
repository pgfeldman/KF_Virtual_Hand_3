#include "StdAfx.h"
#include <rapidxml.hpp>
#include <fstream>
#include "TestManager.h"

using namespace std;
TestManager::TestManager(void)
{
	researcherName[0] = NULL;
	subjectName[0] = NULL;
	subjectAge[0] = NULL;
	subjectGender[0] = NULL;
	testType[0] = NULL;
	seed = 0;
	numOpenLoopTests = 0;
	curOpenLoopTest = 0;

	numTactorTests = 0;
	curTactorTest = 0;

	numHapticTests = 0;
	curHapticTest = 0;

	numHapticTactorTests = 0;
	curHapticTactorTest = 0;

	numSessions = 0;
	curSession = 0;
	curState = STATE_INCOMPLETE;
	testRunning = false;
	startTime = clock();
	resultVector.clear();

	xmin = -200;
	xmax = 200;
	ymin = -100;
	ymax = 150;
	zmin = -100;
	zmax = 10;
	sizemin = 20;
	sizemax = 25;
	goalboxsize = 30;
}

const char *TestManager::setup(HandControlUI *gui){
	if(strcmp(gui->researcherNameInput->value(), "") == 0){
		return "Please set researcherName";
	}else if(strcmp(gui->subjectNameInput->value(), "") == 0){
		return "Please set subjectName";
	}
	sprintf_s(researcherName, "%s", gui->researcherNameInput->value());
	sprintf_s(subjectName, "%s", gui->subjectNameInput->value());
	sprintf_s(subjectAge, "%s", gui->subjectAgeInput->value());
	sprintf_s(subjectGender, "%s", gui->subjectGender->text());
	seed = (int)(gui->seedInput->value());
	srand(seed);
	sprintf_s(testType, "%s", gui->testTypeCombo->text());
	numSessions = (int)(gui->sessionInput->value());
	curSession = 0;

	numOpenLoopTests = 0;
	numHapticTests = 0;
	numTactorTests = 0;
	numHapticTactorTests = 0;
	if(strcmp("All", testType) == 0){
		numOpenLoopTests = numSessions;
		numHapticTests = numSessions;
		numTactorTests = numSessions;
		numHapticTactorTests = numSessions;
	}else{
		if(strcmp("No Feedback", testType) == 0)
			numOpenLoopTests = numSessions;
		if(strstr("Haptic", testType) != NULL)
			numHapticTests = numSessions;
		if(strstr("Tactor", testType) != NULL)
			numTactorTests = numSessions;
		if(strstr("Haptic and Tactor", testType) != NULL)
			numTactorTests = numSessions;
	}

	sprintf_s(strbuf, "0 of %d", numOpenLoopTests);
	gui->curOpenLoopOutput->value(strbuf);		
	
	sprintf_s(strbuf, "0 of %d", numHapticTests);
	gui->curHapticOutput->value(strbuf);

	sprintf_s(strbuf, "0 of %d", numTactorTests);
	gui->curTactorOutput->value(strbuf);

	sprintf_s(strbuf, "0 of %d", numHapticTactorTests);
	gui->curHapticTactorOutput->value(strbuf);

	xmin = (float)gui->xmin->value();
	xmax = (float)gui->xmax->value();
	ymin = (float)gui->ymin->value();
	ymax = (float)gui->ymax->value();
	zmin = (float)gui->zmin->value();
	zmax = (float)gui->zmax->value();

	sizemin = (float)gui->targetSphereMin->value();
	sizemax = (float)gui->targetSphereMax->value();

	goalboxsize = (float)gui->goalBoxSize->value();

	testRunning = false;
	startTime = clock();
	return NULL;
}

void TestManager::loadSetupFile(LPCWSTR fptr, HandControlUI *gui){
	char s[1024];
	string xmlString;
	ifstream infile(fptr);
	if(infile.is_open()){
		while(infile.good()){			
			infile.getline(s, 1024);
			xmlString.append(s);
		}
	}
	infile.close();

	rapidxml::xml_document<> doc;
	doc.parse<0>((char*)xmlString.c_str());
	rapidxml::xml_node<> *setupNode = doc.first_node();
	rapidxml::xml_node<> *node = setupNode->first_node();
	
	while(true){
		printf("node name = %s, node data = %s\n", node->name(), node->value());
		if(strcmp(node->name(), "ResearcherName") == 0)  
			gui->researcherNameInput->value(node->value());
		else if(strcmp(node->name(), "SubjectName") == 0)  
			gui->subjectNameInput->value(node->value());
		else if(strcmp(node->name(), "SubjectAge") == 0)  
			gui->subjectAgeInput->value(node->value());
		else if(strcmp(node->name(), "SubjectGender") == 0){
			if(strcmp(node->value(), "Female") == 0)
				gui->subjectGender->value(1);
			else
				gui->subjectGender->value(0);
		}else if(strcmp(node->name(), "Seed") == 0)  
			gui->seedInput->value(atof(node->value()));
		else if(strcmp(node->name(), "Sessions") == 0)  
			gui->sessionInput->value(atof(node->value()));
		else if(strcmp(node->name(), "GoalBoxSize") == 0)  
			gui->goalBoxSize->value(atof(node->value()));
		else if(strcmp(node->name(), "SphereMin") == 0)  
			gui->targetSphereMin->value(atof(node->value()));
		else if(strcmp(node->name(), "SphereMax") == 0)  
			gui->targetSphereMax->value(atof(node->value()));
		else if(strcmp(node->name(), "Xmin") == 0)  
			gui->xmin->value(atof(node->value()));
		else if(strcmp(node->name(), "Xmax") == 0)  
			gui->xmax->value(atof(node->value()));
		else if(strcmp(node->name(), "Ymin") == 0)  
			gui->ymin->value(atof(node->value()));
		else if(strcmp(node->name(), "Ymax") == 0)  
			gui->ymax->value(atof(node->value()));
		else if(strcmp(node->name(), "Zmin") == 0)  
			gui->zmin->value(atof(node->value()));
		else if(strcmp(node->name(), "Zmax") == 0)  
			gui->zmax->value(atof(node->value()));
		else if(strcmp(node->name(), "Target") == 0)  
			soundFilenameVector.push_back(node->value());
		else if(strcmp(node->name(), "TestType") == 0){
			if(strcmp(node->value(), "No Feedback") == 0)
				gui->testTypeCombo->value(0);
			else if(strcmp(node->value(), "Haptic") == 0)
				gui->testTypeCombo->value(1);
			else if(strcmp(node->value(), "Tactor") == 0)
				gui->testTypeCombo->value(2);
			else if(strcmp(node->value(), "Haptic and Tactor") == 0)
				gui->testTypeCombo->value(3);
			else if(strcmp(node->value(), "All") == 0)
				gui->testTypeCombo->value(4);
		}
		if(node == setupNode->last_node()){
			break;
		}
		node = node->next_sibling();
	}
	setup(gui);

}

/***
	static const int OPEN_LOOP_TEST = 0;
	static const int HAPTIC_TEST = 1;
	static const int TACTOR_TEST = 2;
	static const int HAPTIC_TACTOR_TEST = 3;
	static const int TEST_SESSION_COMPLETE = -1;
/***/

int TestManager::getTestState(){
	const int NUM_STATES = 4;

	int remaining[NUM_STATES];
	remaining[OPEN_LOOP_TEST] = numOpenLoopTests - curOpenLoopTest;
	remaining[HAPTIC_TEST] = numHapticTests - curHapticTest;
	remaining[TACTOR_TEST] = numTactorTests - curTactorTest;
	remaining[HAPTIC_TACTOR_TEST] = numHapticTactorTests - curHapticTactorTest;

	int mostRemainingIndex = TEST_SESSION_COMPLETE;
	int mostRemainingVal = 0;
	float fcount = 0;
	for(int i = 0; i < NUM_STATES; ++i){
		if(remaining[i] > 0){
			if(mostRemainingVal < remaining[i]){
				mostRemainingVal = remaining[i];
				mostRemainingIndex = i;
				fcount = 0;
			}else if (mostRemainingVal == remaining[i]){ // randomly choose among all matching values;
				fcount += 1;
				float odds = 1.0f/fcount;
				if((float)frand(0.0, 1.0) < odds)
					mostRemainingIndex = i;
			}
		}
	}

	return mostRemainingIndex; 
}

void TestManager::startAttempt(){
	//gui->testDirections->value("Place all spheres completely in the cube");
	
	testRunning = true;
	startTime = clock();
}

void TestManager::finishAttempt(KF_Hand_Window *glw, HandControlUI *gui, BasicAudio *ba){
	if(testRunning){ // don't call this multiple times
		addResult(glw, ba);
		testRunning = false;
	}
}

void TestManager::resetTargets(KF_Hand_Window *glw){
	sphereCount = glw->getNumTargets();
	for(int i = 0; i < sphereCount; ++i){
		float size = (float)frand(sizemin, sizemax);
		float x = (float)frand(xmin, xmax);
		float y = (float)frand(ymin, ymax);
		float z = (float)frand(zmin, zmax);
		TargetSphere *ts = glw->getTargetSphere(i);
		ts->setEnabled(true);
		ts->setScalar(size);
		ts->setPostion(x, y, z);
	}
	goalCount = 0;
}

bool TestManager::incrementOpenLoopTest(KF_Hand_Window *glw, HandControlUI *gui){
	curOpenLoopTest++;
	
	sprintf_s(strbuf, "%d of %d", curOpenLoopTest, numOpenLoopTests);
	gui->curOpenLoopOutput->value(strbuf);
	gui->testDirections->value("Open Loop test");
	resetTargets(glw);
	return true;
}

bool TestManager::incrementHapticTest(KF_Hand_Window *glw, HandControlUI *gui){
	curHapticTest++;
	sprintf_s(strbuf, "%d of %d", curHapticTest, numHapticTests);
	gui->curHapticOutput->value(strbuf);
	gui->testDirections->value("Haptic test");
	resetTargets(glw);
	return true;
}

bool TestManager::incrementTactorTest(KF_Hand_Window *glw, HandControlUI *gui){
	curTactorTest++;
	sprintf_s(strbuf, "%d of %d", curTactorTest, numTactorTests);
	gui->curTactorOutput->value(strbuf);
	gui->testDirections->value("Tactor test");
	resetTargets(glw);
	return true;
}

bool TestManager::incrementHapticTactorTest(KF_Hand_Window *glw, HandControlUI *gui){
	curHapticTactorTest++;
	sprintf_s(strbuf, "%d of %d", curHapticTactorTest, numHapticTactorTests);
	gui->curHapticTactorOutput->value(strbuf);
	gui->testDirections->value("Haptic/Tactor test");
	resetTargets(glw);
	return true;
}


void TestManager::addResult(KF_Hand_Window *glw, BasicAudio *ba){
	TestResult *tr = new TestResult();
	tr->sessionNumber = numSessions;
	if(curTest == OPEN_LOOP_TEST){
		tr->testNumber = curOpenLoopTest;
		tr->testType = OPEN_LOOP_TEST;
	}else if(curTest == HAPTIC_TEST){
		tr->testNumber = curHapticTest;
		tr->testType = HAPTIC_TEST;
	}else if(curTest == TACTOR_TEST){
		tr->testNumber = curTactorTest;
		tr->testType = TACTOR_TEST;
	}else if(curTest == HAPTIC_TACTOR_TEST){
		tr->testNumber = curHapticTactorTest;
		tr->testType = HAPTIC_TACTOR_TEST;
	}

	clock_t stopTime = clock();
	float elapsed = ((float)(stopTime - startTime))/CLOCKS_PER_SEC;
	tr->responseTime = elapsed;
	tr->numTargets = glw->getNumTargets();
	tr->numGoals = goalCount;
	resultVector.push_back(tr);

	printf("%s\n", tr->toString());
}

void TestManager::writeFile(LPCWSTR fptr, KF_Hand_Window *kfhw){
	FILE *myFile = _wfopen(fptr, L"w");
	TestResult *tr;
	// current date/time based on current system
   time_t now = time(0);

	// first, we do the csv file
	if(myFile == NULL){
		fl_alert("TestManager::writeFile(): Unable to open file");
		return;
	}else{
		fprintf(myFile, "Date/Time,%s",ctime(&now));
		fprintf(myFile, "Researcher Name,%s\n",researcherName);
		fprintf(myFile, "Subject Name,%s\n", subjectName);
		fprintf(myFile, "Subject Age,%s\n", subjectAge);
		fprintf(myFile, "Subject Gender,%s\n", subjectGender);
		fprintf(myFile, "Seed,%d\n", seed);
		fprintf(myFile, "Open Loop tests,%d\n", numOpenLoopTests);
		fprintf(myFile, "Haptic tests,%d\n", numHapticTests);
		fprintf(myFile, "Tactor tests,%d\n", numTactorTests);
		fprintf(myFile, "Haptic Tactor tests,%d\n", numHapticTactorTests);
		fprintf(myFile, "Sessions,%d\n", numSessions);
		fprintf(myFile, "GoalBoxSize,%.2f\n", goalboxsize);
		fprintf(myFile, "SphereMin,%.2f\n", sizemin);
		fprintf(myFile, "SphereMax,%.2f\n", sizemax);
		fprintf(myFile, "XMin,%.2f\n", xmin);
		fprintf(myFile, "XMax,%.2f\n", xmax);
		fprintf(myFile, "YMin,%.2f\n", xmin);
		fprintf(myFile, "YMax,%.2f\n", xmax);
		fprintf(myFile, "ZMin,%.2f\n", xmin);
		fprintf(myFile, "ZMax,%.2f\n", xmax);
		
		int s = soundFilenameVector.size();		
		for(int i = 0; i < s; ++i){
			string str = soundFilenameVector[i];
			fprintf(myFile, "soundfile %d,%s\n",i, str.c_str());
		}


		s = resultVector.size();
		if(s > 0){
			tr = resultVector[0];
			fprintf(myFile, "Test,%s\n",tr->toCsvHeaderString());
			for(int i = 0; i < s; ++i){
				tr = resultVector[i];
				fprintf(myFile, "%d,%s\n",i, tr->toCsvString());
			}
		}
		fclose(myFile);
	}
	
}

/*****
		else if(strcmp(node->name(), "GoalBoxSize") == 0)  
			gui->goalBoxSize->value(atof(node->value()));
		else if(strcmp(node->name(), "SphereMin") == 0)  
			gui->targetSphereMin->value(atof(node->value()));
		else if(strcmp(node->name(), "SphereMin") == 0)  
			gui->targetSphereMax->value(atof(node->value()));
		else if(strcmp(node->name(), "Xmin") == 0)  
			gui->xmin->value(atof(node->value()));
		else if(strcmp(node->name(), "Xmax") == 0)  
			gui->xmax->value(atof(node->value()));
		else if(strcmp(node->name(), "Ymin") == 0)  
			gui->ymin->value(atof(node->value()));
		else if(strcmp(node->name(), "Ymax") == 0)  
			gui->ymax->value(atof(node->value()));
		else if(strcmp(node->name(), "Zmin") == 0)  
			gui->zmin->value(atof(node->value()));
		else if(strcmp(node->name(), "Zmax") == 0)  
/*****/
void TestManager::writeSetupFile(LPCWSTR fptr, KF_Hand_Window *kfhw){
	FILE *myFile = _wfopen(fptr, L"w");

	// current date/time based on current system
   time_t now = time(0);

	// first, we do the csv file
	if(myFile == NULL){
		fl_alert("TestManager::writeSetupFile(): Unable to open file");
		return;
	}else{
		fprintf(myFile, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n");
		fprintf(myFile, "<Setup>\n");
		string timeStr(ctime(&now));
		timeStr = timeStr.substr(0, timeStr.size()-1);
		fprintf(myFile, "\t<DateTime>%s</DateTime>\n",timeStr.c_str());
		fprintf(myFile, "\t<ResearcherName>%s</ResearcherName>\n",researcherName);
		fprintf(myFile, "\t<SubjectName>%s</SubjectName>\n", subjectName);
		fprintf(myFile, "\t<SubjectAge>%s</SubjectAge>\n", subjectAge);
		fprintf(myFile, "\t<SubjectGender>%s</SubjectGender>\n", subjectGender);
		fprintf(myFile, "\t<Seed>%d</Seed>\n", seed);
		fprintf(myFile, "\t<Sessions>%d</Sessions>\n", numSessions);
		fprintf(myFile, "\t<TestType>%d</TestType>\n", testType);
		fprintf(myFile, "\t<GoalBoxSize>%f</GoalBoxSize>\n", goalboxsize);
		fprintf(myFile, "\t<SphereMin>%f</SphereMin>\n", sizemin);
		fprintf(myFile, "\t<SphereMax>%f</SphereMax>\n", sizemax);
		fprintf(myFile, "\t<Xmin>%f</Xmin>\n", xmin);
		fprintf(myFile, "\t<Xmax>%f</Xmax>\n", xmax);
		fprintf(myFile, "\t<Ymin>%f</Ymin>\n", ymin);
		fprintf(myFile, "\t<Ymax>%f</Ymax>\n", ymax);
		fprintf(myFile, "\t<Zmin>%f</Zmin>\n", zmin);
		fprintf(myFile, "\t<Zmax>%f</Zmax>\n", zmax);
		int numTargets = kfhw->getNumTargets();
		for(int i = 0; i < numTargets; ++i){
			string str = kfhw->getTargetInfo(i);
			fprintf(myFile, "\t<Target>%s</Target>\n", str.c_str());
		}

		fprintf(myFile, "</Setup>\n");

		fclose(myFile);
	}
	// then we do the xml file?
}

TestManager::~TestManager(void)
{
}
