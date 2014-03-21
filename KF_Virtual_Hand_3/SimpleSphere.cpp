#include "StdAfx.h"
#include "SimpleSphere.h"


SimpleSphere::SimpleSphere() : DrawableObject(GL_TEXTURE0)
{
	setup();
}


SimpleSphere::~SimpleSphere(void)
{
}

void SimpleSphere::setup(){
	gltMakeSphere(sphereBatch, 1, 18, 18);
}

void SimpleSphere::render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager){
	modelViewStack.PushMatrix();
		//modelViewStack.Translate(position[0] + size[0]*0.5f, position[1] + size[1]*0.5f, position[2] + size[2]*0.5f);
		modelViewStack.Translate(position[0], position[1], position[2]);
		modelViewStack.Rotate(orientation[1], 0.0f, 1.0f, 0.0f);
		modelViewStack.Rotate(orientation[2], 1.0f, 0.0f, 0.0f);
		modelViewStack.Scale(scalar, scalar, scalar);

		shaderManager.UseStockShader(GLT_SHADER_DEFAULT_LIGHT, modelViewStack.GetMatrix(), projectionStack.GetMatrix(), curColor);	
		sphereBatch.Draw();

	modelViewStack.PopMatrix();
}

void SimpleSphere::environmentCalc(){
}


void SimpleSphere::localCleanup(){
}