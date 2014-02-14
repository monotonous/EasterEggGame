// Egg.h: interface for the egg class.
//////////////////////////////////////////////////////////////////////

#if !defined(EGG_H__INCLUDED_)
#define EGG_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Robot.h"
#include "Geometry.h"

typedef enum {ABOVE_BASKET, OUTSIDE_BASKET, INSIDE_BASKET, BROKEN, CAUGHT} EggStateType;

class CEgg  
{
public:
	CEgg();
	virtual ~CEgg(){}
	void draw();
	void drawPath();
	CVec3df getPosition(){ return CVec3df(position);}
	void init(float posMinRadius, float posMaxRadius);
	void update(float speed);
	EggStateType checkCollision(const CRobot& robot);
	EggStateType state;
private:
	// geometry
	float radius;
	float height;
	GLfloat position[3];
	// material properties
	GLfloat no_shininess[1];
	GLfloat mat_specular[4];
	GLfloat mat_ambient_and_diffuse[4];
};

#endif // !defined(EGG_H__INCLUDED_)
