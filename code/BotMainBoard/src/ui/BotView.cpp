/*
 * BotViewController.cpp
 *
 *  Created on: 07.08.2016
 *      Author: SuperJochenAlt
 */


#include <windows.h>  // openGL windows
#include <thread>

#include <GL/gl.h>
#include <GL/freeglut.h>
#include <GL/glut.h>  // GLUT, includes glu.h and gl.h
#include <GL/Glui.h>

#include "spatial.h"
#include "Util.h"
#include <ui/BotView.h>
#include <ui/BotWindowCtrl.h>

static GLfloat glBotArmColor[] 			= { 1.0f, 0.3f, 0.2f };
static GLfloat glBotJointColor[] 		= { 0.5f, 0.6f, 0.6f };
static GLfloat glCoordSystemColor4v[] 	= { 0.03f, 0.27f, 0.32f,0.5f };
static GLfloat glRasterColor3v[] 		= { 0.73f, 0.77f, 0.82f };

void BotView::setLights()
{
	const float lightDistance = 1500.0f;
  GLfloat light_ambient[] =  {0.2, 0.2, 0.2, 1.0};
  GLfloat light_diffuse[] =  {0.4, 0.4, 0.4, 1.0};
  GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position0[] = {lightDistance, 3*lightDistance, lightDistance, 0.0};		// ceiling left
  GLfloat light_position1[] = {-lightDistance, 3*lightDistance, lightDistance, 0.0};	// ceiling right
  GLfloat light_position2[] = {0, 3*lightDistance, -lightDistance, 0.0};				// far away from the back

  GLfloat mat_ambient[] =  {0.6, 0.6, 0.6, 1.0};
  GLfloat mat_diffuse[] =  {0.4, 0.8, 0.4, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shinynes[] = {50.0};

  glMaterialfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_LIGHT0, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_LIGHT0, GL_SPECULAR, mat_shinynes);
  glMaterialfv(GL_LIGHT1, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_LIGHT1, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_LIGHT1, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_LIGHT1, GL_SPECULAR, mat_shinynes);
  glMaterialfv(GL_LIGHT2, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_LIGHT2, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_LIGHT2, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_LIGHT2, GL_SPECULAR, mat_shinynes);

  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);

  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glLightfv(GL_LIGHT2, GL_AMBIENT, light_ambient);
  glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT2, GL_POSITION, light_position2);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);

  glDepthFunc(GL_LESS);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
}


void BotView::printSubWindowTitle(std::string text, const GLfloat *titleColor ) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();                 // Reset the model-view matrix
	gluOrtho2D(-1.0, 1.0, -1.0, 1.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, titleColor);
	glRasterPos2f(-0.9,0.8);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*) text.c_str());
}


void BotView::drawCoordSystem(bool withRaster, const GLfloat *rasterColor, const GLfloat *axisColor  ) {
	// draw coordinate system
	const float axisLength = 500.0f;
	const float arrowLength = 20.0f;
	const float unitLength = 100.0f;
	const float rasterLineLength = axisLength*2;
	if (withRaster) {
		glPushAttrib(GL_LIGHTING_BIT);
		glBegin(GL_LINES);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, rasterColor);
			glColor3fv(rasterColor);
			for (float i = -rasterLineLength;i<=rasterLineLength;i = i + unitLength ) {
				glVertex3f(i, 0.0, -rasterLineLength);glVertex3f(i,0.0f, rasterLineLength);
			}
			for (float i = -rasterLineLength;i<=rasterLineLength;i = i + unitLength ) {
				glVertex3f(-rasterLineLength, 0.0f, i);glVertex3f(rasterLineLength, 0.0f, i);
			}
		glEnd();
		glPopAttrib();
	}

	glBegin(GL_LINES);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, axisColor);
		glColor4fv(axisColor);

		// robot's x-axis
		glVertex3f(0.0f, 0.0f, -arrowLength);glVertex3f(0.0f, 0.0f, axisLength);
		glVertex3f(0.0f, 0.0f, axisLength);glVertex3f(0.0f,+arrowLength/2, axisLength-arrowLength);
		glVertex3f(0.0f, 0.0f, axisLength);glVertex3f(0.0f,-arrowLength/2, axisLength-arrowLength);
		for (float i = 0;i<axisLength;i = i + unitLength ) {
			glVertex3f(0.0f, -arrowLength/2, i);glVertex3f(0.0f,+arrowLength/2, i);
		}

		// robot's y-axis
		glVertex3f(-arrowLength, 0.0f, 0.0f);glVertex3f(axisLength, 0.0f, 0.0f);
		glVertex3f(axisLength, 0.0f, 0.0f);glVertex3f(axisLength-arrowLength, -arrowLength/2, 0.0f);
		glVertex3f(axisLength, 0.0f, 0.0f);glVertex3f(axisLength-arrowLength, arrowLength/2, 0.0f);
		for (float i = 0;i<axisLength;i = i + unitLength ) {
			glVertex3f(i, -arrowLength/2, 0.0f);glVertex3f(i,+arrowLength/2, 0.0f);
		}

		// robot's z-axis
		glVertex3f(0.0f, -arrowLength, 0.0f);glVertex3f(0.0f, axisLength,0.0f);
		glVertex3f(0.0f, axisLength,0.0f);glVertex3f(+arrowLength/2,axisLength-arrowLength, 0.0f);
		glVertex3f(0.0f, axisLength,0.0f);glVertex3f(-arrowLength/2, axisLength-arrowLength,0.0f);
		for (float i = 0;i<axisLength;i = i + unitLength ) {
			glVertex3f(-arrowLength/2, i,0.0f);glVertex3f(+arrowLength/2, i,0.0f);
		}
	glEnd();

	glRasterPos3f(axisLength+arrowLength, 0.0f, 0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*) "y");
	glRasterPos3f(0.0f, 0.0f, axisLength+arrowLength);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*) "x");
	glRasterPos3f(0.0f, axisLength+arrowLength,0.0f);
	glutBitmapString(GLUT_BITMAP_HELVETICA_12,(const unsigned char*) "z");
}



void BotView::paintBot(JointAngleType angles, const GLfloat *subWindowColor) {
	const float baseplateRadius= 140;
	const float baseplateHeight= 20;

	const float baseLength = 110;
	const float baseRadius = 60;
	const float baseJointRadius = 60;

	const float upperarmLength = 210;
	const float upperarmJointRadius= 45;
	const float upperarmRadius = 45;

	const float forearmLength = 240;
	const float forearmJointRadius= 35;
	const float forearmRadius = 35;

	const float handLength= 90;
	const float handJointRadius= 23;
	const float handRadius= 23;

	const float gripperLength= 70;
	const float gripperRadius=10;

	const float gripperLeverLength= 45;
	const float gripperLeverRadius=5;

	glMatrixMode(GL_MODELVIEW);
	glClearColor(subWindowColor[0], subWindowColor[1],subWindowColor[2],0.0f); // Set background color to white and opaque

	// coord system
	drawCoordSystem(true, glRasterColor3v, glCoordSystemColor4v);

	// base plate
	glPushMatrix();
	glRotatef(-90.0,1.0,0.0, 0.0);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
	glutSolidCylinder(baseplateRadius, baseplateHeight, 36, 1);

	// shoulder
	glTranslatef(0.0, 0.0,baseplateHeight);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);
	glutSolidCylinder(baseRadius, baseLength, 36, 1);

	// shoulder joint
	glTranslatef(0.0,0.0,baseLength);  // Move right and into the screen
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
	glutSolidSphere(baseJointRadius, 36, 36);

	// upperarm
	glRotatef(angles[0],0.0,0.0, 1.0); // turn along angle
	glRotatef(angles[1],1.0,0.0, 0.0); // rotate along base angle
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);
	glutSolidCylinder(upperarmRadius, upperarmLength, 36, 1);

	// upperarm joint
	glTranslatef(0.0,0.0,upperarmLength);  // move to its start height
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
	glutSolidSphere(upperarmJointRadius, 36, 36);

	// forearm
	glRotatef(90+angles[2],1.0,0.0, 0.0); // rotate along base angle
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);
	glutSolidCylinder(forearmRadius, forearmLength, 36, 1);

	// forearm joint
	glRotatef(angles[3],0.0,0.0, 1.0); // rotate along base angle
	glTranslatef(0.0,0.0,forearmLength);  // move to its start height
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
	glPushMatrix(),
		glTranslatef(forearmJointRadius/2,0.0,0);  // move to its start height
		glutSolidSphere(forearmJointRadius, 36, 36);
	glPopMatrix();
	glPushMatrix(),
		glTranslatef(-forearmJointRadius/2,0.0,0);  // move to its start height
		glutSolidSphere(forearmJointRadius, 36, 36);
	glPopMatrix();
	glPushMatrix(),
		glTranslatef(-forearmJointRadius/2,0.0,0);  // move to its start height
		glRotatef(90,0.0,1.0, 0.0); // rotate along base angle
		glutSolidCylinder(forearmJointRadius, forearmJointRadius, 36, 1);
	glPopMatrix();

	// hand
	glRotatef(angles[4],1.0,0.0, 0.0); // rotate along base angle
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);
	glutSolidCylinder(handRadius, handLength, 36, 1);

	// hand joint
	glTranslatef(0.0,0.0,handLength);  // move to its start height
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
	glutSolidSphere(handJointRadius, 36, 36);

	// hand
	glRotatef(angles[5],0.0,0.0, 1.0); // rotate along base angle
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);

	glPushMatrix();
		glRotatef(angles[6],0.0,1.0, 0.0); // rotate along base angle
		glutSolidCylinder(gripperLeverRadius, gripperLeverLength, 36, 1);
		glTranslatef(0,0.0,gripperLeverLength);
		glRotatef(-angles[6],0.0,1.0, 0.0); // rotate along base angle
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
		glutSolidSphere(gripperRadius, 36, 36);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);
		glutSolidCylinder(gripperRadius, gripperLength, 36, 1);
	glPopMatrix();
	glPushMatrix();
		glRotatef(-angles[6],0.0,1.0, 0.0); // rotate along base angle
		glutSolidCylinder(gripperLeverRadius, gripperLeverLength, 36, 1);
		glTranslatef(0,0.0,gripperLeverLength);
		glRotatef(angles[6],0.0,1.0, 0.0); // rotate along base angle
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotJointColor);
		glutSolidSphere(gripperRadius, 36, 36);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, glBotArmColor);
		glutSolidCylinder(gripperRadius, gripperLength, 36, 1);
	glPopMatrix();

	glPopMatrix();
}