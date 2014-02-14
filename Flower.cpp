// Flower.cpp: implementation of the CFlower class.
//////////////////////////////////////////////////////////////////////

#include "Flower.h"
#include "Utilities.h"
#include <gl/glut.h>

CFlower::CFlower()
{
	// initialise geometry parameters
	nSlices=8;
	nStacks=8;
	angleWithXAxis=(GLfloat) randomDouble(0,90);	// in degree (!)
	stemRadius=0.02;
	stemHeight=(GLfloat) randomDouble(0.3,0.5);
	leafLength=stemHeight/2;
	leafAngle=(GLfloat) randomDouble(30,70);	// in degree (!)
	leafHeight1=stemHeight/3.5;
	leafHeight2=stemHeight/2.5;
	blossomCentreRadius=stemHeight/6;
	petalLength=2*blossomCentreRadius;
	numPetals=10;
	// initialise material parameters
	no_shininess[0]=10;
	mat_specular[0]=0.2;
	mat_specular[1]=0.2;
	mat_specular[2]=0.2;
	mat_specular[3]=1;
	mat_stem[0]=0.2f;
	mat_stem[1]=0.5f;
	mat_stem[2]=0.1f;
	mat_stem[3]=1;
	mat_blossom_centre[0]=0.7f;
	mat_blossom_centre[1]=0.65f;
	mat_blossom_centre[2]=0.15f;
	mat_blossom_centre[3]=1;
	mat_petal[0]=0.65f;
	mat_petal[1]=0.25f;
	mat_petal[2]=0.15f;
	mat_petal[3]=1;
}

void CFlower::draw()
{
	glEnable(GL_NORMALIZE);
	glPushMatrix();
	glTranslatef(x,0,z);	// move flower onto the ground plane
	glRotatef(90-angleWithXAxis, 0, 1, 0);	// rotate flower within the ground plane
	glRotatef(-90,1,0,0);	// rotate flower upright

	// draw entire flower aligned with z axis
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
	// draw stem
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_stem);
	glutSolidCone(stemRadius, stemHeight, nSlices, nStacks);
	// draw leaf 1
	glPushMatrix();
	glTranslatef(0, 0, leafHeight1);
	glRotatef(leafAngle, 1, 0, 0);
	glTranslatef(0, 0, leafLength/2);
	glScalef(0.1, 0.4, 1);
	glutSolidSphere(leafLength/2, nSlices, nStacks);
	glPopMatrix();
	// draw leaf 2
	glPushMatrix();
	glTranslatef(0, 0, leafHeight2);
	glRotatef(-leafAngle, 1, 0, 0);
	glTranslatef(0, 0, leafLength/2);
	glScalef(0.1, 0.4, 1);
	glutSolidSphere(leafLength/2, nSlices, nStacks);
	glPopMatrix();
	//draw blossom
	glTranslatef(0, 0, stemHeight);		// translates blossom AND pedals
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_blossom_centre);
	glPushMatrix();
	glScalef(0.4, 1, 1);
	glutSolidSphere(blossomCentreRadius, nSlices, nStacks);
	glPopMatrix();
	// draw petals
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, mat_petal);
	for(int i=0;i<numPetals;i++)
	{
		glPushMatrix();
		glRotatef(360.0/numPetals*i, 1, 0, 0);
		glTranslatef(0, 0, petalLength/2);
		glScalef(0.2, 0.5, 1);
		glutSolidSphere(petalLength/2, nSlices, nStacks);
		glPopMatrix();
	}
	glPopMatrix();
	glDisable(GL_NORMALIZE);
}
