#pragma once
#include "drawableobject.h"
#include "PhidgetsController.h"
#include <FL/Fl_Value_Slider.H>

class Gripper :
	public DrawableObject
{
public:
	Gripper(void);
	~Gripper(void);

	void setup();
	void render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager); // don't use!
	void render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, float *phantomMat, GLShaderManager &shaderManager);
	void environmentCalc(); // don't use!
	void environmentCalc(GLMatrixStack &modelViewStack, float phantomMat[16]);
	void localCleanup();

	float *getGripperOffest(int index){
		return offsets[index];
	}

	float *getGripperPosition(int index){
		return positions[index];
	}

	double getGripperForce(int index){
		return endEffector[index].gripperForce;
	}

	void setContactForce(int index, double force){
		endEffector[index].contactForce = force;
	}

	void setEffectorScalar(int index, double val){
		endEffector[index].scalar = val;
	}


	static const int RED = 0;
	static const int GREEN = 1;
	static const int RED2 = 2;
	static const int RED3 = 3;
	static const int RED4 = 4;
	static const int GREEN2 = 5;
	static const int GREEN3 = 6;
	static const int GREEN4 = 7;
	static const int YELLOW = 8;
	
	static const int NUM_END_EFFECTORS = 9;

private: 
	GLTriangleBatch     sphereBatch;
	GLTriangleBatch     cylBatch;

	float  offsets[NUM_END_EFFECTORS][3];
	float  positions[NUM_END_EFFECTORS][3];

	GLuint	flatShader;
	GLint	flatLocMVP;				// The location of the ModelViewProjection matrix uniform
	GLint	flatLocColorValue;		// The location of the color
	GLint	flatLocDissolveFactor;	// The location of the dissolve effect value
	GLint   flatLocTexture;			// The location of the texture

	GLuint	testShader;			// The diffuse light shader

	PhidgetsController phidgets;
	
	typedef struct {
		int minRaw;
		int maxRaw;
		int currentRaw;
		double scalar;
		double gripperForce;
		double restoreForce;
		double minRange;
		double maxRange;
		double curPos;
		double contactForce;
	} END_EFFECTOR;

	END_EFFECTOR endEffector[NUM_END_EFFECTORS];
	Fl_Value_Slider *sliders[2];
};

