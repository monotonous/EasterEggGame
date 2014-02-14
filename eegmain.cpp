//=============================================================
//= Author: Joshua Parker                                     =
//=                                                           =
//= Egg catching game                                         =
//= Use the robotic arm to catch the falling eggs             =
//=                                                           =
//= Keys:                                                     =
//= +/- to zoom in/out                                        =
//= h - display egg "hint" lines                              =
//= w - move arm counter-clockwise                            =
//= z - move arm clockwise                                    =
//= a - move arm out                                          =
//=	s - move arm in                                           =
//=                                                           =
//=============================================================
#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glut.h>
#include <time.h>

#include "Trackball.h"
#include "Utilities.h"
#include "Geometry.h"
#include "Mirror.h"
#include "GroundWithGrass.h"
#include "Robot.h"
#include "Flower.h"
#include "Egg.h"
#include "Lighting.h"

const int windowWidth=800;
const int windowHeight=800;

clock_t currentSystemTime, previousSystemTime;
double timePassed, keyPressTime;

bool W_DOWN, Z_DOWN, A_DOWN, S_DOWN;
clock_t time_w_down, time_z_down, time_a_down, time_s_down;

CTrackball trackball;
CMirror mirror;
CLighting lighting;
CRobot robot;
CGroundWithGrass ground;
const int numFlowers=20;
const float minDistanceFlowers=0.2f;
CFlower flowers[numFlowers];

const int numEggs=10;	// maximum number of eggs at the same time
						// if an egg is caught or hit it is reinitialised
CEgg eggs[numEggs];

static GLuint texName[4];

// game score
int numEggsHit=0;
int numEggsMissed=0;
char hitString[20]="   0 hits";
char missString[20]="   0 misses";
bool gameOver=false;
float fov=16.0f;

bool enablePathVisualization=false;

void drawScene()
{
	int i;

	// draw ground with grass
	ground.draw();

	glShadeModel(GL_SMOOTH);
	lighting.enable();
	// draw eggs
	for(i=0;i<numEggs;i++) eggs[i].draw();
	// draw flowers
	for(i=0;i<numFlowers;i++) flowers[i].draw();
	// draw robot
	robot.draw();
	lighting.disable();

	// draw path lines to help catching eggs
	glLineWidth(2.0);
	if (enablePathVisualization)
		for(i=0;i<numEggs;i++) eggs[i].drawPath();
	
	// draw projected shadows of the eggs, flowers and the robot
	// Note: this is just a quick hack
	glColor3f(0.09, 0.12, 0.09);
	glPushMatrix();
	GLfloat* lightPosition=lighting.getLight(0)->getPosition();
	double d = lightPosition[1];
	glTranslatef(lightPosition[0], lightPosition[1]+0.08f, lightPosition[2]);
	double projectionMatrix[] = {d,0,0,0,  0,d,0,-1,  0,0,d,0,  0,0,0,0};
	glMultMatrixd(projectionMatrix);
	glTranslatef(-lightPosition[0], -lightPosition[1], -lightPosition[2]);
	// draw eggs
	for(i=0;i<numEggs;i++) eggs[i].draw();
	// draw flowers
	for(i=0;i<numFlowers;i++) flowers[i].draw();
	// draw robot
	robot.draw();
	glPopMatrix();

}

void drawStringAtRasterPosition(char *string, int x, int y)
{
		int viewport[4];
		double mvmatrix[16];
		double projmatrix[16];
		double p[3];
		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
		glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
		gluUnProject(x ,y, 0.001, mvmatrix, projmatrix, viewport, &p[0], &p[1], &p[2]);
		glRasterPos3d(p[0],p[1],p[2]);
		for(char *s=string;*s!='\0';*s++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *s);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_MODELVIEW );	// Set the view matrix ...
	glLoadIdentity();				// ... to identity.
	trackball.tbMatrix();
	glEnable(GL_DEPTH_TEST);

	// draw scene with mirror image
	GLdouble plane_equation[]={0.0, 0.0, 1.0, 2.01};
	glClipPlane(GL_CLIP_PLANE0,plane_equation);
	glEnable(GL_CLIP_PLANE0);					// clip off objects behind the mirror (e.g. shadows of eggs)
	drawScene();								// draw scene
	glDisable(GL_CLIP_PLANE0);
	mirror.drawMirrorImage(drawScene,lighting);	// draw mirror image of scene

	// draw score
	lighting.disable();
	glShadeModel(GL_FLAT);
	glColor3f(0.0,0.0,0.0);
	drawStringAtRasterPosition(hitString, 50, 760);
	drawStringAtRasterPosition(missString, 50, 740);

	if (gameOver){
		char scoreString[40];
		sprintf_s(scoreString,"Congratulations, your score is: %d :-)",numEggsHit);
		drawStringAtRasterPosition(scoreString, 50, 720);
	}

	glFlush ();
	glutSwapBuffers();
}

void animate(void)
{
	if (!gameOver){
		currentSystemTime = clock();
		timePassed = ((double)(currentSystemTime - previousSystemTime))/((double) CLOCKS_PER_SEC);
		if (timePassed>0.0013)
		{
			for(int i=0;i<numEggs;i++){
				eggs[i].update(0.01);
				EggStateType state=eggs[i].checkCollision(robot);
				if (state==BROKEN){
					numEggsMissed++;
					sprintf_s(missString,"%4d misses",numEggsMissed);
					CVec3df p=eggs[i].getPosition();
					ground.addSmashedEgg(p[0],p[2]);
					eggs[i].init(2*robot.basketRadius, 2*robot.armLength+robot.basketRadius);
					if (numEggsMissed>50) gameOver=true;
				}
				else if (state==CAUGHT){
					numEggsHit++;
					if (numEggsHit <= 1)
						sprintf_s(hitString,"%4d hit",numEggsHit);
					else
						sprintf_s(hitString,"%4d hits",numEggsHit);
					eggs[i].init(2*robot.basketRadius, 2*robot.armLength+robot.basketRadius);
				}
			}

			if (W_DOWN){
				keyPressTime=(double)(clock() - time_w_down);	// in ms
				robot.incrementPhi(keyPressTime);
			}
			if (Z_DOWN){
				keyPressTime=(double)(clock() - time_z_down);	// in ms
				robot.decrementPhi(keyPressTime);
			}
			if (A_DOWN){
				keyPressTime=(double)(clock() - time_a_down);	// in ms
				robot.incrementTheta(keyPressTime);
			}
			if (S_DOWN){
				keyPressTime=(double)(clock() - time_s_down);	// in ms
				robot.decrementTheta(keyPressTime);
			}
			robot.updateBasketPosition();
			glutPostRedisplay();
		}
	}
}

void _initView()
{
	// initialize view (simple perspective projection)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, (GLfloat) windowWidth/(GLfloat) windowHeight, 15.0, 35.0);
	gluLookAt(0,0,25, 0,0.75f,0, 0,1,0);
}

void init(void)
{
	// select clearing color (for glClear)
	glClearColor (1.0, 1.0, 1.0, 0.0);	// RGB-value for white

	// initialize view (simple perspective projection)
	_initView();

	trackball.tbInit(GLUT_LEFT_BUTTON);

	lighting.init();

	// Distribute flowers randomly over the region outside the robot arms reach such that
	// the minimum distance between any two flowers is at least minDistanceFlowers
	int i;
	bool validPosition;
	float x,z,xx,zz;
	for(i=0;i<numFlowers;i++)
	{
		do {
			validPosition=true;
			x=(GLfloat) randomDouble(-2.45,2.45);
			z=(GLfloat) randomDouble(-1.8,1.95);
			if (sqrt(x*x+z*z)<(2*CRobot::armLength+2*CRobot::basketRadius+0.2)){validPosition=false; continue;}
			for(int j=0;j<i;j++)
			{	// position is valid if the distance between the stems of each tree is at least minDistanceFlowers
				flowers[j].getPosition(xx,zz);
				if (sqrt((xx-x)*(xx-x)+(zz-z)*(zz-z))<minDistanceFlowers) { validPosition=false; break;}
			}
		} while (validPosition==false);
		flowers[i].setPosition(x,z);
	}

	// initialise eggs
	sgenrand((unsigned long)clock());
	for(i=0;i<numEggs;i++)
		eggs[i].init(2*CRobot::basketRadius, 2*CRobot::armLength+CRobot::basketRadius);

	// initialise textures of the grassy ground and the robot basket
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(2, texName);
	ground.setTexture(texName[0]);
	robot.setTexture(texName[1]);
}

void reshape(int width, int height ) {
	// Called at start, and whenever user resizes component
	int size = min(width, height);
	glViewport(0, 0, size, size);  // Largest possible square
	trackball.tbReshape(width, height);
}

void handleMouseMotion(int x, int y)
{
	trackball.tbMotion(x, y);
}

void handleMouseClick(int button, int state, int x, int y)
{
	trackball.tbMouse(button, state, x, y);
}

void handleKeyPress(unsigned char key, int x, int y)
{
	if ((key=='w') && (!W_DOWN)) { W_DOWN=true; time_w_down = clock();}
	if ((key=='z') && (!Z_DOWN)) { Z_DOWN=true; time_z_down = clock();}
	if ((key=='a') && (!A_DOWN)) { A_DOWN=true; time_a_down = clock();}
	if ((key=='s') && (!S_DOWN)) { S_DOWN=true; time_s_down = clock();}
	if ((key=='h') || (key=='H')) enablePathVisualization=!enablePathVisualization;
	// ======================================================================
	// = Handles routines for zooming and changing the basket shape.        =
	// ======================================================================
	if ((key=='=') || (key=='+')){
		if (fov > 1) fov /= 1.1;
		_initView();
		glutPostRedisplay();
	}
	if ((key=='-') || (key=='_')){
		if (fov < 80) fov *= 1.1;
		_initView();
		glutPostRedisplay();
	}
	if ((key=='r') || (key=='R')) robot.changeBasketShape();
}

void handleKeyRelease(unsigned char key, int x, int y)
{
	if (key=='w') W_DOWN=false;
	if (key=='z') Z_DOWN=false;
	if (key=='a') A_DOWN=false;
	if (key=='s') S_DOWN=false;
}

// create a single buffered colour window
int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Easter Egg Game");
	init ();						// initialise view
	glutSetKeyRepeat(0);
	glutMouseFunc(handleMouseClick);		// Set function to handle mouse clicks
	glutMotionFunc(handleMouseMotion);		// Set function to handle mouse motion
	glutKeyboardFunc(handleKeyPress);		// Set function to handle pressing of key
	glutKeyboardUpFunc(handleKeyRelease);	// Set function to handle release of key
	glutDisplayFunc(display);		// Set function to draw scene
	glutReshapeFunc(reshape);		// Set function called if window gets resized
	glutIdleFunc(animate);
	glutMainLoop();
	return 0;
}
