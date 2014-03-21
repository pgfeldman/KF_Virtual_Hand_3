#pragma once
#include "drawableobject.h"
#include "TargetSphere.h"

class GoalBox :
	public DrawableObject
{
public:
	GoalBox(void);
	~GoalBox(void);

	void setup();
	void render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager);
	bool isTargetInside(TargetSphere *ts);
	bool isTargetTouching(TargetSphere *ts);
	void environmentCalc();
	void localCleanup();

protected: 
	GLBatch				boxBatch;
};

