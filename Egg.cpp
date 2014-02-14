// Egg.cpp: implementation of the CEgg class.
// Author: Joshua Parker
//
// Enabled the egg lines "cheat"
//////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <time.h>

#include "Egg.h"
#include "Geometry.h"
#include "Utilities.h"

CEgg::CEgg()
{
	// initialise material properties
	no_shininess[0]=10;
	mat_specular[0]=0.4;
	mat_specular[1]=0.4;
	mat_specular[2]=0.4;
	mat_specular[3]=1;
	mat_ambient_and_diffuse[0]=0.85f;
	mat_ambient_and_diffuse[1]=0.85f;
	mat_ambient_and_diffuse[2]=0.1f;
	mat_ambient_and_diffuse[3]=1;

	radius=0.07;
	height=0.1;
	state=ABOVE_BASKET;
}

void CEgg::init(float posMinRadius, float posMaxRadius)
{
	// initialise position
	double radius=(GLfloat) randomDouble(posMinRadius, posMaxRadius);
	double angle=randomDouble(0, 2*Pi);
	position[1]=(GLfloat) randomDouble(5, 15);
	position[0]=(GLfloat) (radius*cos(angle));
	position[2]=(GLfloat) (radius*sin(angle));
}

void CEgg::update(float speed)
{
	position[1]-=speed;
}

EggStateType CEgg::checkCollision(const CRobot& robot)
{
	// If egg is hitting rim it breaks
	if (position[1]>robot.armLength+height) state=ABOVE_BASKET;
	else if (position[1]>robot.armLength){
		float distance=sqrt(fsqr(position[0]-robot.basketCentre[0])+fsqr(position[2]-robot.basketCentre[2]));
		if (distance < (robot.basketRadius-radius)){
			if (state==ABOVE_BASKET) state=INSIDE_BASKET;
			if (state==OUTSIDE_BASKET) state=BROKEN;	// rim of basket moved through egg
		}
		else if (distance < (robot.basketRadius+radius)) state=BROKEN; // egg intersects rim of basket
		else{ // distance > (robot.basketRadius+radius)
			if (state==INSIDE_BASKET) state=BROKEN;		// rim of basket moved through egg
			else state=OUTSIDE_BASKET;
		}
	}
	else{ // position[1]<robot.armLength
		if (position[1]<height) state=BROKEN;			// egg has hit ground
		float distance=sqrt(fsqr(position[0]-robot.basketCentre[0])+fsqr(position[2]-robot.basketCentre[2]));
		if (distance < (robot.basketRadius-radius))
			if (state==INSIDE_BASKET) state=CAUGHT;		// egg has fallen through the rim of basket and now inside mesh
	}
	return state;
}


void CEgg::draw()
{
	glEnable(GL_NORMALIZE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_ambient_and_diffuse);

	glPushMatrix();
	glTranslatef(position[0], position[1], position[2]);
	glScalef(radius, height, radius);
	glutSolidSphere(1.0, 12, 8);
	glPopMatrix();
	glDisable(GL_NORMALIZE);
}

void CEgg::drawPath()
{
	//==================================================================
	//= Draws a line from the centre of the egg straight down to the   =
	//= ground plane. The line is coloured blue (0.3, 0.3, 1.0) if the =
	//= distance is larger than 3 units, orange (1.0, 0.5, 0.3) if the =
	//= distance is between 1.5 and 3 units and yellow (1.0, 1.0, 0.3) =
	//= otherwise.                                                     =
	//==================================================================
	if (position[1] > 3)
		glColor3f(0.3f, 0.3f, 1.0f);
	else if (position[1] > 1.5)
		glColor3f(1.0f, 0.5f, 0.3f);
	else
		glColor3f(1.0f, 1.0f, 0.3f);

	glBegin(GL_LINES);
	glVertex3fv(position);
	glVertex3f(position[0], 0, position[2]);
	glEnd();
}

