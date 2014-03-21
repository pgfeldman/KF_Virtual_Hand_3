#include "StdAfx.h"
#include "GoalBox.h"


GoalBox::GoalBox(void) : DrawableObject(GL_TEXTURE0)
{
	setup();
}


GoalBox::~GoalBox(void)
{
}

void GoalBox::setup(){

	gltMakeCube(boxBatch, 1);
}

void GoalBox::render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager){
	modelViewStack.PushMatrix();
		//modelViewStack.Translate(position[0] + size[0]*0.5f, position[1] + size[1]*0.5f, position[2] + size[2]*0.5f);
		modelViewStack.Translate(position[0], position[1], position[2]);
		modelViewStack.Rotate(orientation[1], 0.0f, 1.0f, 0.0f);
		modelViewStack.Rotate(orientation[2], 1.0f, 0.0f, 0.0f);
		modelViewStack.Scale(scalar, scalar, scalar);

		shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
		boxBatch.Draw();

	modelViewStack.PopMatrix();
}

bool GoalBox::isTargetInside(TargetSphere *ts){
	if(!ts->isEnabled())
		return false;

	float tscalar = ts->getScalar();
	float center[3];
	center[0] = ts->getXpos();
	center[1] = ts->getYpos();
	center[2] = ts->getZpos();


	minAARB[0] = position[0] - scalar + tscalar;
	minAARB[1] = position[1] - scalar + tscalar;
	minAARB[2] = position[2] - scalar + tscalar;

	maxAARB[0] = position[0] + scalar - tscalar;
	maxAARB[1] = position[1] + scalar - tscalar;
	maxAARB[2] = position[2] + scalar - tscalar;

	float seg[3];
	bool retval = true;
	for(int i = 0; i < 3; ++i){
		seg[i] = (minAARB[i] - center[i]) * (maxAARB[i] - center[i]);
		if((seg[i] > 0) && (retval == true))
			retval = false;
	}

	//Dprint::add("isTargetInside: seg = (%.2f, %.2f, %.2f) retval = %d", seg[0], seg[1], seg[2], retval);

	return retval;
}

bool GoalBox::isTargetTouching(TargetSphere *ts){
	if(!ts->isEnabled())
		return false;

	float tscalar = ts->getScalar();
	float center[3];
	center[0] = ts->getXpos();
	center[1] = ts->getYpos();
	center[2] = ts->getZpos();


	minAARB[0] = position[0] - scalar - tscalar;
	minAARB[1] = position[1] - scalar - tscalar;
	minAARB[2] = position[2] - scalar - tscalar;

	maxAARB[0] = position[0] + scalar + tscalar;
	maxAARB[1] = position[1] + scalar + tscalar;
	maxAARB[2] = position[2] + scalar + tscalar;

	float seg[3];
	bool retval = true;
	for(int i = 0; i < 3; ++i){
		seg[i] = (minAARB[i] - center[i]) * (maxAARB[i] - center[i]);
		if((seg[i] > 0) && (retval == true))
			retval = false;
	}

	//Dprint::add("isTargetTouching: seg = (%.2f, %.2f, %.2f) retval = %d", seg[0], seg[1], seg[2], retval);

	return retval;
}

void GoalBox::environmentCalc(){
}


void GoalBox::localCleanup(){
}