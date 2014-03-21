#pragma once

#include <string>
#include <vector>
#include <tuple>

using namespace std;

typedef tuple<GLfloat, GLfloat, string> TWOD_GLF_TUPLE;
typedef tuple<float, float, float, string> FOURF_SAMPLE_TUPLE;

class TestResult
{
public:
	TestResult(void);
	~TestResult(void);

	const char *toString();
	const char *toCsvString();
	const char *toCsvHeaderString();

	static const char *testLabel[];

	int sessionNumber;
	int testNumber;
	int testType;
	float responseTime;
	int numTargets;
	int numGoals;


private:
	char strbuf[2048];
	string textString;
	string xmlString;
	string csvString;
	string csvHeaderString;
};

