#pragma once
#include "drawableobject.h"

class SimpleSphere :
	public DrawableObject
{
public:
	SimpleSphere(void);
	~SimpleSphere(void);

	void setup();
	void render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager);
	void environmentCalc();
	void localCleanup();

protected: 
	GLTriangleBatch     sphereBatch;
};

