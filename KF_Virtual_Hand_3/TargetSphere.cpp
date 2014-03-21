#include "StdAfx.h"
#include "TargetSphere.h"


TargetSphere::TargetSphere(void)
{
	for(int i = 0; i < 3; ++i){
		constraintAnchorPos[i] = 0;
		velocityVec[i] = 0;
		for(int j = 0; j < SimToPhantom::NUM_SENSORS; ++j)
			sensorPos[j][i] = 0;
	}
	for(int i = 0; i < MAX_SOUNDS; ++i)
		soundArray[i] = NULL;
	drag = 0.9;
	gripRatio = 0.3; 
	
	phantomToSim = NULL;
	continuousSound = NULL;
	enabled = true;
}


TargetSphere::~TargetSphere(void)
{
}

void TargetSphere::render(GLMatrixStack &modelViewStack, GLMatrixStack &projectionStack, GLShaderManager &shaderManager){
	if(enabled)
		SimpleSphere::render(modelViewStack, projectionStack, shaderManager);

}

void TargetSphere::environmentCalc(){
	double forces = phantomToSim->targForcesMagnitude[targIndex];
	double sumVec[3];
	double sumMag = 0;

	sumVec[0] = sumVec[1] = sumVec[2] = 0;
	for(int i = 0; i < SimToPhantom::NUM_SENSORS; ++i){
		double force = phantomToSim->targForceMagnitude[targIndex][i];
		sumMag += force;
		double forceVec[3];
		forceVec[0] = phantomToSim->targForceVec[targIndex][i][0];
		forceVec[1] = phantomToSim->targForceVec[targIndex][i][1];
		forceVec[2] = phantomToSim->targForceVec[targIndex][i][2];
		sumVec[0] += forceVec[0];
		sumVec[1] += forceVec[1];
		sumVec[2] += forceVec[2];

		//Dprint::add("targ[%d] sens[%d] forceVec = (%.2f., %.2f, %.2f) force = %.2f, forces = %.2f",
		//	targIndex, i, forceVec[0], forceVec[1], forceVec[2], force, forces);
	}

	double len = sqrt(sumVec[0]*sumVec[0] + sumVec[1]*sumVec[1] + sumVec[2]*sumVec[2]);
	double unit[3];
	for(int i = 0; i < 3; ++i){
		if(len == 0)
			unit[i] = 0;
		else
			unit[i] = sumVec[i]/len;
	}


	//Dprint::add("sumVec (%.2f, %.2f, %.2f), unit*len(%.2f, %.2f, %.2f)
	double ratio = 1.0;
	if(sumMag >0)
		ratio = forces/sumMag;
	//Dprint::add("Target[%d]: ratio = %.2f, drag = %.2f", targIndex, ratio, drag*ratio);

	for(int i = 0; i < 3; ++i){
		velocityVec[i] -= sumVec[i];

		position[i] += (float)velocityVec[i];
		if(velocityVec[i] > drag*ratio){
			velocityVec[i] -= drag*ratio;
		}else if(velocityVec[i] < -drag*ratio){
			velocityVec[i] += drag*ratio;
		}else{
			velocityVec[i] = 0;
		}
	}
}