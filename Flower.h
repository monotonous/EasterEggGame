// Flower.h: interface for the CFlower class.
//////////////////////////////////////////////////////////////////////

#if !defined(FLOWER_H__INCLUDED_)
#define FLOWER_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <gl/gl.h>

class CFlower  
{
public:
	CFlower();
	virtual ~CFlower(){}
	void draw();
	void setPosition(GLfloat x, GLfloat z){ this->x=x; this->z=z;}
	void getPosition(GLfloat& x, GLfloat& z){ x=this->x; z=this->z;}
private:
	GLfloat x,z;	// (x,z) position of the tree on the ground plane
	// geometry parameters
	int nSlices;
	int nStacks;
	int numPetals;
	double stemRadius;
	double stemHeight;
	double leafLength;
	double leafAngle;
	double leafHeight1;
	double leafHeight2;
	double blossomCentreRadius;
	double petalLength;
	double angleWithXAxis;
	// material properties
	GLfloat no_shininess[1];
	GLfloat mat_specular[4];
	GLfloat mat_stem[4];
	GLfloat mat_blossom_centre[4];
	GLfloat mat_petal[4];
};

#endif // !defined(FLOWER_H__INCLUDED_)
