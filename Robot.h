// Robot.h: interface for the robot class.
// Author: Joshua Parker
//////////////////////////////////////////////////////////////////////

#if !defined(ROBOT_H__INCLUDED_)
#define ROBOT_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Geometry.h"

const int NUM_LONGITUDINAL_VERTICES=10;
const int NUM_CIRCUMFERENTIALL_VERTICES=16;

class CRobot  
{
public:
	CRobot();
	virtual ~CRobot(){}
	void draw();
	void setTexture(GLuint i){ texName=i; _initTexture(); }
	void incrementPhi(double time){ phi+=0.3*(1+0.02*time); if (phi>360) phi-=360;}
	void decrementPhi(double time){ phi-=0.3*(1+0.02*time); if (phi<0) phi+=360;}
	void incrementTheta(double time){ theta+=0.3*(1+0.02*time); if (theta>thetaMax) theta=thetaMax;}
	void decrementTheta(double time){ theta-=0.3*(1+0.02*time); if (theta<thetaMin) theta=thetaMin;}
	// geometry
	static const double rimRadius;
	static const double basketRadius;
	static const double basketHeight;
	static const double armLength;
	static const double bottomArmWidth;
	static const double topArmWidth;
	double basketShapeAngle;
	void updateBasketPosition();
	float basketCentre[3];
	// basket
	void changeBasketShape();
private:
	// material properties
	GLfloat no_shininess[1];
	GLfloat mat_specular[4];
	GLfloat mat_ambient_and_diffuse[4];
	// animation parameters
	double thetaMin, thetaMax;
	double phi, theta;	// angles for rotating robot arms
	double timeLastStep;
	// Hermite Curve
	CVec2df hermiteCurve(double t);
	// attributes and methods for the basket at the tip of the robot arm
	void _initBasketSurface();
	void _drawBasket();
	float vertices[NUM_LONGITUDINAL_VERTICES][NUM_CIRCUMFERENTIALL_VERTICES][3];
	// texture parameters
	void _initTexture();
	float texture_coordinates[NUM_LONGITUDINAL_VERTICES][NUM_CIRCUMFERENTIALL_VERTICES][2];
	GLuint texName;
	GLubyte *texture;
	int textureWidth, textureHeight;
};

#endif // !defined(ROBOT_H__INCLUDED_)
