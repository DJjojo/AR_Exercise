
#define GLFW_INCLUDE_GLU
#include <glfw/glfw3.h>
#include "DrawPrimitives.h"
#include <cstdio>
using namespace std;


void display(GLFWwindow* window){
	float ratio;
	int width, height;

	glfwGetFramebufferSize(window, &width, &height);
	ratio = width / (float)height;

	glViewport(0, 0, width, height);

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//  BONUS: Orthographic camera for playing around	
	//	glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
	gluPerspective(30, ((double)width / (double)height), 0.01, 100);

	// move to origin
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	
	// move the object backwards
	glTranslatef(0.0, -2.0, -10.0);

	
	glRotatef((float)glfwGetTime()*50.f,0.f,1.f,0.f);
	glColor4f(1.0, 1.0, 1.0, 1.0);

	glTranslatef(0.0, 1.0, 0.0);
	drawSphere(0.5, 30, 30);
	glTranslatef(0.0, 0.6, 0.0);
	drawSphere(0.35, 30, 30);
	glTranslatef(0.0, 0.4, 0.0);
	drawSphere(0.2, 30, 30);

	glPushMatrix();
	glColor4f(0.0, 0.0, 0.0, 1.0);
	glTranslatef(0.1, 0.05, 0.11);
	drawSphere(0.05, 8, 8);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(-0.1, 0.05, 0.11);
	drawSphere(0.05, 8, 8);

	glPopMatrix();
	glTranslatef(0.0, 0.02, 0.2);
	glColor4f(1.0, 0.5, 0.0, 1.0);
	drawCone(0.05, 0.15, 8, 4);

}
void resize(GLFWwindow* window, int width, int height){
	// set a whole-window viewport
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// create a perspective projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(30, ((GLfloat)width / (GLfloat)height), 0.01, 100);

}


int main()
{
	glfwInit();

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", nullptr, nullptr);

	glfwSetFramebufferSizeCallback(window, resize);
	glfwSwapInterval(1);
	glfwMakeContextCurrent(window);

	glEnable(GL_COLOR_MATERIAL);
	glClearColor(0.4, 0.51, 0.9, 1.0);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	glEnable(GL_LIGHTING);

	GLfloat light_position[] = { 10.0, 5.0, 0.0, 0.0 };	   //Werte für Lichtposition
	GLfloat ambientLight[] = { 0.3, 0.3, 0.3, 1.0 };	       //Werte für ambientes Licht
	GLfloat diffuseLight[] = { 0.8, 0.8, 0.8, 1.0 };			   //Werte für diffuses Licht
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		   //Initsialisierung der Lichter und der Position
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHT0);									   //Lichtquelle einschalten
	while (!glfwWindowShouldClose(window))
	{
		
		display(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}


