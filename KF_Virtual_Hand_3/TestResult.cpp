#include "StdAfx.h"

#include <FL/gl.h>
#include "TestResult.h"
#define _USE_MATH_DEFINES
#include <math.h>


using namespace std;

const char* TestResult::testLabel[] = {"OPEN_LOOP", "HAPTIC", "TACTOR", "HAPTIC_TACTOR"};

TestResult::TestResult(void)
{
	sessionNumber = 0;
	testNumber = 0;
	testType = 0;
	numTargets = 0;
	numGoals = 0;
}


TestResult::~TestResult(void)
{
}


const char *TestResult::toString(){
	textString.clear();
	textString.append("\n");
	sprintf_s(strbuf, "Num tests: %d\n", sessionNumber);	textString.append(strbuf);
	sprintf_s(strbuf, "Test Number: %d\n", testNumber);	textString.append(strbuf);
	sprintf_s(strbuf, "Test Type: %s\n", testLabel[testType]);	textString.append(strbuf);
	sprintf_s(strbuf, "Response Time: %f\n", responseTime);	textString.append(strbuf);
	sprintf_s(strbuf, "Num Targets: %d\n", numTargets);	textString.append(strbuf);
	sprintf_s(strbuf, "Num Goals: %d\n", numGoals);	textString.append(strbuf);
	
	return textString.c_str();
}


const char *TestResult::toCsvString(){
	csvString.clear();
	sprintf_s(strbuf, "%d,", sessionNumber);	csvString.append(strbuf);
	sprintf_s(strbuf, "%d,", testNumber);	csvString.append(strbuf);
	sprintf_s(strbuf, "%s,", testLabel[testType]);	csvString.append(strbuf);
	sprintf_s(strbuf, "%f,", responseTime);	csvString.append(strbuf);
	sprintf_s(strbuf, "%d,", numTargets);	csvString.append(strbuf);
	sprintf_s(strbuf, "%d,", numGoals);	csvString.append(strbuf);
	
	textString.append(strbuf);
	return csvString.c_str();
}

const char *TestResult::toCsvHeaderString(){
	csvHeaderString.clear();
	csvHeaderString.append("TotalTests,TestNum,TestType,ResponseTime,NumTargets,NumGoals");
	
	return csvHeaderString.c_str();
}
