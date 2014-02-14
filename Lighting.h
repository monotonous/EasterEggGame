// Lighting.h: interface for the CLighting and CLight class.
//////////////////////////////////////////////////////////////////////

#if !defined(LIGHTING__INCLUDED_)
#define LIGHTING__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <gl/gl.h>


class CLight
{
public:
	friend class CLighting;
	CLight(int id);
	virtual ~CLight(){}
	void setPosition(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	GLfloat* getPosition(){return light_position;}
	void init();
private:
	int glLightID;
	GLfloat light_position[4];
	GLfloat light_ambient[4];
	GLfloat light_diffuse[4];
	GLfloat light_specular[4];
};


class CLighting
{
public:
	CLighting();
	virtual ~CLighting(){ delete[] lights;}
	void enable() const;
	void disable() const;
	void init() const;
	CLight* getLight(int i){ return lights[i];}
private:
	int numLights;
	CLight **lights;
};


#endif // !defined(LIGHTING__INCLUDED_)
