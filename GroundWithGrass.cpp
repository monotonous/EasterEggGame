// GroundWithGrass.cpp: implementation of the CGroundWithGrass class.
//////////////////////////////////////////////////////////////////////

#define LINE_ANTIALIASING

#include <fstream>
using namespace std;
#include "GroundWithGrass.h"
#include "Utilities.h"

CGroundWithGrass::CGroundWithGrass()
{
	groundY=0;
	groundX1=-2.5; groundX2=2.5;
	groundZ1=2; groundZ2=-2;
	p0=CVec3df(groundX1, groundY, groundZ1);
	p1=CVec3df(groundX2, groundY, groundZ1);
	p2=CVec3df(groundX2, groundY, groundZ2);
	p3=CVec3df(groundX1, groundY, groundZ2);

	plane=CPlane3df(p0,p1,p2);

	t0=CVec2df(0,0);
	t1=CVec2df(1,0);
	t2=CVec2df(1,1);
	t3=CVec2df(0,1);
}

CGroundWithGrass::~CGroundWithGrass()
{

}

void CGroundWithGrass::_init()
{
	// load grass texture
	ifstream textureFile;
	char* fileName="grass2.ppm";
	textureFile.open(fileName, ios::in);
	if (textureFile.fail())
		displayMessage(ERROR_MESSAGE, "initGround(): could not open file %s",fileName);

	skipLine(textureFile);
	skipLine(textureFile);
	textureFile >> textureWidth;
	textureFile >> textureHeight;
	int numRGBValues;
	textureFile >> numRGBValues;
	texture = new GLubyte[textureWidth*textureHeight*4];

	int k,l,r,g,b;
	for(k=0;k<textureHeight;k++)
		for(l=0;l<textureWidth;l++)
		{
			textureFile >> r;
			texture[(k*textureWidth+l)*4]=(GLubyte) r;
			textureFile >> g;
			texture[(k*textureWidth+l)*4+1]=(GLubyte) g;
			textureFile >> b;
			texture[(k*textureWidth+l)*4+2]=(GLubyte) b;
			texture[(k*textureWidth+l)*4+3]=(GLubyte) 255;
		}
	textureFile.close();

	glBindTexture(GL_TEXTURE_2D, texName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 
				0, GL_RGBA, GL_UNSIGNED_BYTE, texture);	

	// load egg texture
	fileName="SmashedEgg.ppm";
	textureFile.open(fileName, ios::in);
	if (textureFile.fail())
		displayMessage(ERROR_MESSAGE, "initGround(): could not open file %s",fileName);

	skipLine(textureFile);
	textureFile >> eggTextureWidth;
	textureFile >> eggTextureHeight;
	textureFile >> numRGBValues;
	eggTexture = new GLubyte[eggTextureWidth*eggTextureHeight*4];

	for(k=0;k<eggTextureHeight;k++)
		for(l=0;l<eggTextureWidth;l++)
		{
			textureFile >> r;
			eggTexture[(k*eggTextureWidth+l)*4]=(GLubyte) r;
			textureFile >> g;
			eggTexture[(k*eggTextureWidth+l)*4+1]=(GLubyte) g;
			textureFile >> b;
			eggTexture[(k*eggTextureWidth+l)*4+2]=(GLubyte) b;
			if ((r==0) && (g==0) && (b=255))
				eggTexture[(k*eggTextureWidth+l)*4+3]=(GLubyte) 0;
			else
				eggTexture[(k*eggTextureWidth+l)*4+3]=(GLubyte) 255;
		}
	textureFile.close();

	// draw grass blades
	for(int i=0;i<NUM_GRASS_BLADES;i++)
	{
		float width=(float) randomDouble(0.01f, 0.04f);
		float x1=(float) randomDouble(groundX1, groundX2);
		float x2=(float) randomDouble(x1, fmin(x1+width,groundX2));
		float z1=(float) randomDouble(groundZ2, groundZ1);
		float z2=(float) randomDouble(z1, fmin(z1+width,groundZ1));
		grassBlade3DPositions[i][0][0]=x1,
		grassBlade3DPositions[i][0][1]=groundY,
		grassBlade3DPositions[i][0][2]=z1,
		grassBlade3DPositions[i][1][0]=x2,
		grassBlade3DPositions[i][1][1]=groundY+0.05f,
		grassBlade3DPositions[i][1][2]=z2,
		grassBlade3DTextureCoordinates[i][0][0]=(x1-groundX1)/(groundX2-groundX1);
		grassBlade3DTextureCoordinates[i][0][1]=(z1-groundZ1)/(groundZ2-groundZ1);
		grassBlade3DTextureCoordinates[i][1][0]=(x2-groundX1)/(groundX2-groundX1);
		grassBlade3DTextureCoordinates[i][1][1]=(z2-groundZ1)/(groundZ2-groundZ1);
	}
}

void CGroundWithGrass::addSmashedEgg(float x, float z)
{
	// compute texture coordinate where egg hits the ground and copy texture image of a smashed egg in there
	int xOffset=(int) (((x-groundX1)/(groundX2-groundX1))*textureWidth)-eggTextureWidth/2;
	int yOffset=(int) (((z-groundZ1)/(groundZ2-groundZ1))*textureHeight)-eggTextureHeight/2;

	int k,l; 
	for(k=0;k<eggTextureHeight;k++)
		for(l=0;l<eggTextureWidth;l++)
			if (eggTexture[(k*eggTextureWidth+l)*4+3]>0){
				texture[((k+yOffset)*textureWidth+(l+xOffset))*4]=eggTexture[(k*eggTextureWidth+l)*4];
				texture[((k+yOffset)*textureWidth+(l+xOffset))*4+1]=eggTexture[(k*eggTextureWidth+l)*4+1];
				texture[((k+yOffset)*textureWidth+(l+xOffset))*4+2]=eggTexture[(k*eggTextureWidth+l)*4+2];
				texture[((k+yOffset)*textureWidth+(l+xOffset))*4+3]=eggTexture[(k*eggTextureWidth+l)*4+3];
			}
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureWidth, textureHeight, 
				0, GL_RGBA, GL_UNSIGNED_BYTE, texture);	

}

void CGroundWithGrass::draw()
{
	glColor3f(0,1,0.9f);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texName);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	glTexCoord2fv(t0.getArray()); glVertex3fv(p0.getArray());
	glTexCoord2fv(t1.getArray()); glVertex3fv(p1.getArray());
	glTexCoord2fv(t2.getArray()); glVertex3fv(p2.getArray());
	glTexCoord2fv(t3.getArray()); glVertex3fv(p3.getArray());
	glEnd();

#ifdef LINE_ANTIALIASING
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glHint(GL_LINE_SMOOTH_HINT, GL_FASTEST);
#endif
	glBegin(GL_LINES);
	for(int i=0;i<NUM_GRASS_BLADES;i++)
	{
		glTexCoord2fv(grassBlade3DTextureCoordinates[i][0]); glVertex3fv(grassBlade3DPositions[i][0]);
		glTexCoord2fv(grassBlade3DTextureCoordinates[i][1]); glVertex3fv(grassBlade3DPositions[i][1]);
	}
	glEnd();
#ifdef LINE_ANTIALIASING
	glDisable(GL_LINE_SMOOTH);
	glDisable(GL_BLEND);
#endif
	
	glDisable(GL_TEXTURE_2D);
	glFlush();
}
