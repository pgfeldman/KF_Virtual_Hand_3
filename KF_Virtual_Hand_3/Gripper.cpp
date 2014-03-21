#include "StdAfx.h"
#include "Gripper.h"



Gripper::Gripper() : DrawableObject(GL_TEXTURE0)
{
	setup();
}


Gripper::~Gripper(void)
{
}

void Gripper::setup(){
	phidgets.start();

	gltMakeSphere(sphereBatch, 1, 18, 18);
	gltMakeCylinder(cylBatch, 1, 1, 1, 18, 2);

	for(int i = 0; i < YELLOW; ++i){
		offsets[i][0] = offsets[i][1] = offsets[i][2] = 0;
	}

	for(int i = 0; i < NUM_END_EFFECTORS; ++i){
		endEffector[i].minRaw = 410;
		endEffector[i].maxRaw = 480;
		endEffector[i].scalar = .3;
		endEffector[i].gripperForce = 0;
		endEffector[i].restoreForce = 0.4;
		endEffector[i].minRange = 1.0;
		endEffector[i].maxRange = 4.0;
		endEffector[i].curPos = 4.0;
		endEffector[i].contactForce = 0;
	}

}

void Gripper::render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager){
}

void Gripper::render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, float *phantomMat, GLShaderManager &shaderManager){
		modelViewStack.PushMatrix();
		//modelViewStack.Translate(position[0] + size[0]*0.5f, position[1] + size[1]*0.5f, position[2] + size[2]*0.5f);

		modelViewStack.MultMatrix(phantomMat);
		modelViewStack.Scale(scalar, scalar, scalar);

		shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
		sphereBatch.Draw();

		modelViewStack.PushMatrix();			
			modelViewStack.PushMatrix();
				modelViewStack.Rotate(90, 0, 1, 0);
				modelViewStack.Scale(0.5, 0.5, endEffector[RED].curPos);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
				cylBatch.Draw();
			modelViewStack.PopMatrix();

			modelViewStack.Translate(endEffector[RED].curPos, 0, 0.0);
			shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vRed);	
			sphereBatch.Draw();

			modelViewStack.Translate(0.0, 0, -8.0);
			modelViewStack.PushMatrix();
				modelViewStack.Scale(0.5, 0.5, 8.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
				cylBatch.Draw();
			modelViewStack.PopMatrix();
			shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vRed);	
			sphereBatch.Draw();
			modelViewStack.PushMatrix();
				modelViewStack.Translate(0.0, 2.0, -2.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vRed);	
				sphereBatch.Draw();
				modelViewStack.Translate(0.0, -4.0, 0.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vRed);	
				sphereBatch.Draw();
				modelViewStack.Translate(0.0, 2.0, -2.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vRed);	
				sphereBatch.Draw();
			modelViewStack.PopMatrix();
		modelViewStack.PopMatrix();

		modelViewStack.PushMatrix();	
			modelViewStack.PushMatrix();
				modelViewStack.Rotate(-90, 0, 1, 0);
				modelViewStack.Scale(0.5, 0.5, endEffector[GREEN].curPos);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
				cylBatch.Draw();
			modelViewStack.PopMatrix();

			modelViewStack.Translate(-endEffector[GREEN].curPos, 0, 0.0);
			shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vGreen);	
			sphereBatch.Draw();

			modelViewStack.Translate(0.0, 0, -8.0);
			modelViewStack.PushMatrix();
				modelViewStack.Scale(0.5, 0.5, 8.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
				cylBatch.Draw();
			modelViewStack.PopMatrix();
			
			shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vGreen);	
			sphereBatch.Draw();
			modelViewStack.PushMatrix();
				modelViewStack.Translate(0.0, 2.0, -2.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vGreen);	
				sphereBatch.Draw();
				modelViewStack.Translate(0.0, -4.0, 0.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vGreen);	
				sphereBatch.Draw();
				modelViewStack.Translate(0.0, 2.0, -2.0);
				shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), vGreen);	
				sphereBatch.Draw();
			modelViewStack.PopMatrix();
		modelViewStack.PopMatrix();


	modelViewStack.PopMatrix();
}

void Gripper::environmentCalc(){
}

void Gripper::environmentCalc(GLMatrixStack &modelViewStack, float phantomMat[16]){
	phidgets.sampleAll();

	for(int i = 0; i < 2; ++i){
		if(phidgets.getRaw(i) > 500)
			return;
		if(phidgets.getRaw(i) < 0)
			return;
	}

	endEffector[RED].currentRaw = phidgets.getRaw(0);
	endEffector[GREEN].currentRaw = phidgets.getRaw(1);
	for(int i = 0; i < 2; ++i){
		END_EFFECTOR *ee = &(endEffector[i]);
		ee->maxRaw = max(ee->maxRaw, ee->currentRaw);
		ee->minRaw = min(ee->minRaw, ee->currentRaw);
		double range = ee->maxRaw - ee->minRaw;
		double raw = ee->currentRaw - ee->minRaw;

		ee->gripperForce = max((1.0 - (raw/range)), 0);
		ee->curPos = ee->curPos - (ee->gripperForce - ee->restoreForce - ee->contactForce) * ee->scalar;

		ee->curPos = min(ee->maxRange, ee->curPos);
		ee->curPos = max(ee->minRange, ee->curPos);
		//Dprint::add("[%d] range = %.2f, raw = %.2f, maxRaw = %d, minRaw = %d, curPos = %.2f", i, range, raw, ee->maxRaw, ee->minRaw, ee->curPos);
	}

	modelViewStack.PushMatrix();
		modelViewStack.LoadIdentity();
		modelViewStack.MultMatrix(phantomMat);
		modelViewStack.Scale(scalar, scalar, scalar);
		modelViewStack.GetMatrix(matrix);
		setPosFromMatrix(positions[YELLOW]);

		modelViewStack.PushMatrix();			
			modelViewStack.Translate(endEffector[RED].curPos, 0, 0.0);
			modelViewStack.Translate(0.0, 0, -8.0);
			modelViewStack.GetMatrix(matrix);
			setPosFromMatrix(positions[RED]);
			modelViewStack.PushMatrix();
				modelViewStack.Translate(0.0, 2.0, -2.0);
				modelViewStack.GetMatrix(matrix);
				setPosFromMatrix(positions[RED2]);
				modelViewStack.Translate(0.0, -4.0, 0.0);
				modelViewStack.GetMatrix(matrix);
				setPosFromMatrix(positions[RED3]);
				modelViewStack.Translate(0.0, 2.0, -2.0);
				modelViewStack.GetMatrix(matrix);
				setPosFromMatrix(positions[RED4]);
			modelViewStack.PopMatrix();
		modelViewStack.PopMatrix();

		modelViewStack.PushMatrix();	
			modelViewStack.Translate(-endEffector[GREEN].curPos, 0, 0.0);
			modelViewStack.Translate(0.0, 0, -8.0);
			modelViewStack.GetMatrix(matrix);
			setPosFromMatrix(positions[GREEN]);
			modelViewStack.PushMatrix();
				modelViewStack.Translate(0.0, 2.0, -2.0);
				modelViewStack.GetMatrix(matrix);
				setPosFromMatrix(positions[GREEN2]);
				modelViewStack.Translate(0.0, -4.0, 0.0);
				modelViewStack.GetMatrix(matrix);
				setPosFromMatrix(positions[GREEN3]);
				modelViewStack.Translate(0.0, 2.0, -2.0);
				modelViewStack.GetMatrix(matrix);
				setPosFromMatrix(positions[GREEN4]);
			modelViewStack.PopMatrix();
		modelViewStack.PopMatrix();

	modelViewStack.PopMatrix();

	for(int i = 0; i < NUM_END_EFFECTORS; ++i){
		offsets[i][0] = positions[i][0] - positions[YELLOW][0];
		offsets[i][1] = positions[i][1] - positions[YELLOW][1];
		offsets[i][2] = positions[i][2] - positions[YELLOW][2];
	}
}

void Gripper::localCleanup(){
	phidgets.terminate();
}