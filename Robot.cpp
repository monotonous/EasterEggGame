// Robot.cpp: implementation of the CRobot class.
// Author: Joshua Parker
//////////////////////////////////////////////////////////////////////

#include <time.h>
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include "Robot.h"
#include "Geometry.h"
#include "Utilities.h"

#include <iostream>
#include <fstream>
using namespace std;

const double CRobot::rimRadius=0.02;
const double CRobot::basketRadius=0.2;
const double CRobot::basketHeight=0.4;
const double CRobot::armLength=0.6;
const double CRobot::bottomArmWidth=0.2;
const double CRobot::topArmWidth=0.1;

// ======================================================================
// = Defines the profile curve of the basket.                          =
// ======================================================================
double hermite0(double t){return t*t*(2*t-3)+1;} 
double hermite1(double t){return t*t*(3-2*t);} 
double hermite2(double t){return t*t*(t-2)+t;} 
double hermite3(double t){return t*t*(t-1);}

CVec2df CRobot::hermiteCurve(double t){
	CVec2df p1(basketHeight, 0);
	CVec2df p4(0, basketRadius);
	CVec2df r1(0, 2*basketRadius);
	CVec2df r4(-2*basketHeight*cos(2*Pi*basketShapeAngle/360.0),
		-2*basketHeight*sin(2*Pi*basketShapeAngle/360.0));
	
	return hermite0(t) * p1 + hermite1(t) * p4 + hermite2(t) * r1 + hermite3(t) * r4;
}

void CRobot::changeBasketShape(){
	basketShapeAngle += 1.0;
	if (basketShapeAngle > 360) basketShapeAngle -= 360.0;
	_initBasketSurface();
	glutPostRedisplay();
} 

void CRobot::_initBasketSurface(){
	float z, r;
	// ===========================================================
	// = Represents the basket at the tip of the robot arm       =
	// ===========================================================
	for(int i = 0; i < NUM_LONGITUDINAL_VERTICES; i++){
		CVec2df p = hermiteCurve((double) i / (double) (NUM_LONGITUDINAL_VERTICES-1));
		z = p[0]; 
		r = p[1];
		//for static basket
		//z = basketHeight*cos(Pi/2*((double) i/(NUM_LONGITUDINAL_VERTICES-1))); 
		//r = basketRadius*sin(Pi/2*((double) i/(NUM_LONGITUDINAL_VERTICES-1))); 
		for(int j = 0; j < NUM_CIRCUMFERENTIALL_VERTICES; j++){
			vertices[i][j][0] = (float) (cos(2*Pi*j/(double)(NUM_CIRCUMFERENTIALL_VERTICES-1))*r);
			vertices[i][j][1] = (float) (sin(2*Pi*j/(double)(NUM_CIRCUMFERENTIALL_VERTICES-1))*r);
			vertices[i][j][2] = z;
			texture_coordinates[i][j][1] = (double)i/(NUM_LONGITUDINAL_VERTICES-1);
			texture_coordinates[i][j][0] = (double)j/(NUM_CIRCUMFERENTIALL_VERTICES-1);
		}
	}
}

CRobot::CRobot()
{
	// initialise material properties
	no_shininess[0]=5;
	mat_specular[0]=0.1;
	mat_specular[1]=0.1;
	mat_specular[2]=0.1;
	mat_specular[3]=1;
	mat_ambient_and_diffuse[0]=0.15f;
	mat_ambient_and_diffuse[1]=0.25f;
	mat_ambient_and_diffuse[2]=0.8f;
	mat_ambient_and_diffuse[3]=1;

	// initialise geometry
	thetaMin=10.0;
	thetaMax=90.0;
	basketShapeAngle=0.0;

	// initialise animation
	phi=0.0;
	theta=thetaMin;

	// initialise basket geometry
	_initBasketSurface();
}


void CRobot::_initTexture()
{	// load net texture
	ifstream textureFile;
	char* fileName="Net.pgm";

	// =============================================================
	// = Loads the texture image and initialise the texture map    =
	// =============================================================
	int rgbVal;
	textureFile.open(fileName, ios::in);
	skipLine(textureFile);
	textureFile >> textureWidth;
	textureFile >> textureHeight;
	textureFile >> rgbVal;
	texture = new GLubyte[textureWidth * textureHeight * 4];

	for(int i = 0; i < textureHeight; i++){
		for(int j = 0; j < textureWidth; j++){
			int textSize;
			int textWidth = i * textureWidth + j;
			textureFile >> textSize;
			texture[textWidth * 4] = (GLubyte) textSize;
			texture[textWidth * 4 + 1] = (GLubyte) textSize;
			texture[textWidth * 4 + 2] = (GLubyte) textSize;
			if(textSize == 255) 
				texture[textWidth * 4 + 3] = (GLubyte) 0;
			else 
				texture[textWidth * 4 + 3] = (GLubyte) 255;
		}
	}
	textureFile.close();

	glBindTexture(GL_TEXTURE_2D, texName); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight,0, GL_RGBA, GL_UNSIGNED_BYTE, texture);
	delete[] texture;
}

void CRobot::updateBasketPosition()
{
	float radius=2*armLength*sin(theta*Pi/180.0f)+basketRadius;
	basketCentre[0]=radius*sin((phi-90)*Pi/180.0f);
	basketCentre[1]=armLength;
	basketCentre[2]=radius*cos((phi-90)*Pi/180.0f);
}

void CRobot::_drawBasket()
{
	glShadeModel(GL_FLAT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glAlphaFunc(GL_GREATER, 0.9f);
	glEnable(GL_ALPHA_TEST);

	// =====================================================
	// = Draws the basket as a surface of revolution       =
	// = Texture map of basket with an image of the net    =
	// =====================================================
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	
	for(int i = 0; i < NUM_LONGITUDINAL_VERTICES-1; i++){
		glBegin(GL_QUAD_STRIP);
		for(int j = 0; j < NUM_CIRCUMFERENTIALL_VERTICES; j++){
			glTexCoord2fv(texture_coordinates[i][j]);
			glVertex3fv(vertices[i][j]);
			glTexCoord2fv(texture_coordinates[i + 1][j]);
			glVertex3fv(vertices[i + 1][j]);
		}
		glEnd();
	}
	
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);
}

void CRobot::draw()
{
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);

	// rotate the whole robot
	glPushMatrix();
	glRotatef(phi, 0, 1, 0);
	// base of arm
	glPushMatrix();
	glScalef(bottomArmWidth, armLength, bottomArmWidth);
	glTranslatef(0, 0.5, 0);
	glutSolidCube(1.0);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(0, armLength, 0);
	glRotatef(theta, 0, 0, 1);
	// middle of arm
	glPushMatrix();
	glScalef(topArmWidth, armLength, topArmWidth);
	glTranslatef(0, 0.5, 0);
	glutSolidCube(1.0);
	glPopMatrix();
	// top of arm
	glTranslatef(0, armLength, 0);
	glRotatef(180-2*theta, 0, 0, 1);
	glPushMatrix();
	glScalef(topArmWidth, armLength, topArmWidth);
	glTranslatef(0, 0.5, 0);
	glutSolidCube(1.0);
	glPopMatrix();
	// ring and basket on top
	glTranslatef(0, armLength, 0);
	glRotatef(-90+theta, 0, 0, 1);
	glTranslatef(0, basketRadius, 0);
	glRotatef(-90, 0, 1, 0);
	glutSolidTorus(rimRadius, basketRadius, 16, 16);
	_drawBasket();
	glPopMatrix();
	// fixes the shadow rotating with everything problem - matches top push
	glPopMatrix();
	glDisable(GL_NORMALIZE);
	glFlush();
}
