// Mirror.h: interface for the CMirror class.
//////////////////////////////////////////////////////////////////////

#if !defined(MIRROR_H__INCLUDED_)
#define MIRROR_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <gl/gl.h>
#include "Geometry.h"
#include "Lighting.h"

typedef enum {MIRROR_FRONT, MIRROR_BACK, MIRROR_BOUNDARY} PART_OF_MIRROR;

class CMirror
{
public:
	CMirror();
	virtual ~CMirror(){}
	void drawMirrorImage(void (*drawScene)(), const CLighting& lighting);
private:
	bool showMirror;
	void drawMirror(PART_OF_MIRROR type);
	void initReflectionMatrix();
	void initWorldToMirrorTransformation();
	void applyReflectionMatrix();
	CVec3df p0,p1,p2,p3,n;
	GLfloat reflectionMatrix[4][4];
	GLfloat mirrorToWorldTransformation[4][4];
	GLfloat worldToMirrorTransformation[4][4];
};


#endif // !defined(MIRROR_H__INCLUDED_)
