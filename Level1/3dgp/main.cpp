#include <iostream>
#include "GL/glew.h"
#include "GL/3dgl.h"
#include "GL/glut.h"
#include "GL/freeglut_ext.h"

// Include GLM core features
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#pragma comment (lib, "glew32.lib")

using namespace std;
using namespace _3dgl;
using namespace glm;

unsigned vertexBuffer = 0;
unsigned normalBuffer = 0;
unsigned indexBuffer = 0;

float vertices[] = {
	-4, 0, -4, 4, 0, -4, 0, 7, 0, -4, 0, 4, 4, 0, 4, 0, 7, 0,
	-4, 0, -4, -4, 0, 4, 0, 7, 0, 4, 0, -4, 4, 0, 4, 0, 7, 0,
	-4, 0, -4, -4, 0, 4, 4, 0, -4, 4, 0, 4 };
float normals[] = {
	0, 4, -7, 0, 4, -7, 0, 4, -7, 0, 4, 7, 0, 4, 7, 0, 4, 7,
	-7, 4, 0, -7, 4, 0, -7, 4, 0, 7, 4, 0, 7, 4, 0, 7, 4, 0,
	0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0 };
unsigned indices[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 13, 14, 15 };


// 3D models
C3dglModel camera;
C3dglModel table;
C3dglModel vase;
C3dglModel teapot;
C3dglModel helmet;
C3dglModel room;

C3dglProgram Program;

// camera position (for first person type camera navigation)
mat4 matrixView;			// The View Matrix
float angleTilt = 15;		// Tilt Angle
float angleRot = 0.1f;		// Camera orbiting angle
vec3 cam(0);				// Camera movement values

bool init()
{
	// rendering states
	glEnable(GL_DEPTH_TEST);	// depth test is necessary for most 3D scenes
	glEnable(GL_NORMALIZE);		// normalization is needed by AssImp library models
	glShadeModel(GL_SMOOTH);	// smooth shading mode is the default one; try GL_FLAT here!
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	// this is the default one; try GL_LINE!

	// setup lighting


	//Initialise Shaders
	C3dglShader VertexShader;
	C3dglShader FragmentShader;

	if (!VertexShader.Create(GL_VERTEX_SHADER))return false;
	if (!VertexShader.LoadFromFile("shaders/basic.vert"))return false;
	if (!VertexShader.Compile())return false;

	if (!FragmentShader.Create(GL_FRAGMENT_SHADER))return false;
	if (!FragmentShader.LoadFromFile("shaders/basic.frag"))return false;
	if (!FragmentShader.Compile())return false;

	if (!Program.Create())return false;
	if (!Program.Attach(VertexShader))return false;
	if (!Program.Attach(FragmentShader))return false;
	if (!Program.Link())return false;
	if (!Program.Use(true))return false;

	// prepare vertex data
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// prepare normal data
	glGenBuffers(1, &normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);

	// prepare indices array
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);



	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!teapot.load("models\\teapot.obj")) return false;
	if (!helmet.load("models\\Mandalorian.obj")) return false;
	if (!room.load("models\\LivingRoom.obj")) return false;

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(0.0, 5.0, 25.0),
		vec3(0.0, 5.0, 0.0),
		vec3(0.0, 1.0, 0.0));




	// setup the screen background colour
	glClearColor(0.18f, 0.25f, 0.22f, 1.0f);   // deep grey background

	cout << endl;
	cout << "Use:" << endl;
	cout << "  WASD or arrow key to navigate" << endl;
	cout << "  QE or PgUp/Dn to move the camera up and down" << endl;
	cout << "  Shift+AD or arrow key to auto-orbit" << endl;
	cout << "  Drag the mouse to look around" << endl;
	cout << endl;

	return true;
}

void done()
{
}

void renderScene(mat4 &matrixView, float time)
{
	mat4 m;

	Program.SendUniform("matrixView", matrixView); //ESSENTIAL FOR DIRECTIONAL LIGHT

	//send the material colour (red, green, blue)... in one go
	Program.SendUniform("materialSpecular", 0.0, 0.0, 0.0);
	// camera
	m = matrixView;
	m = translate(m, vec3(-3.0f, 5, 25.0f));
	m = rotate(m, radians(225.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.04f, 0.04f, 0.04f));
	camera.render(m);

	Program.SendUniform("materialSpecular", 0.6, 0.6, 0.6);
	//table
	m = matrixView;
	m = translate(m, vec3(20.0f, -16.7, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.020f, 0.020f, 0.020f));
	table.render(1, m);

	//chairs
	for (int i = 0; i < 4; i++) 
	{
		float tx = 20.0f, ty = -16.7f, tz = 0.0, rotation = 180.f;
		if (i == 1) 
		{
			tx = 20.0f; ty = -16.7; tz = 0.0f; rotation = 0.f;
		}
		if (i == 2)
		{
			tx = 25.0f; ty = -16.7; tz = 1.f; rotation = 90.f;
		}
		if (i == 3)
		{
			tx = 15.0f; ty = -16.7; tz = 1.f; rotation = -90.f;
		}
		m = matrixView;
		m = translate(m, vec3(tx, ty, tz));
		m = rotate(m, radians(rotation), vec3(0.0f, 1.0f, 0.0f));
		m = scale(m, vec3(0.020f, 0.020f, 0.020f));
		table.render(0, m);
	}

	Program.SendUniform("materialSpecular", 0.0, 0.0, 1.0);
	// vase
	m = matrixView;
	m = translate(m, vec3(20.0f, -1.5f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.4f, 0.4f, 0.4f));
	vase.render(m);

	Program.SendUniform("materialSpecular", 0.0, 1.0, 0.0);
	// teapot
	m = matrixView;
	m = translate(m, vec3(15.0f, -1.5f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(2.f, 2.f, 2.f));
	teapot.render(m);

	Program.SendUniform("materialSpecular", 1.0, 0.0, 0.0);
	// helmet
	m = matrixView;
	m = translate(m, vec3(25.0f, -1.5f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.15f, 0.15f, 0.15f));
	helmet.render(m);

	Program.SendUniform("materialSpecular", 1.0, 1.0, 1.0);
	//room
	m = matrixView;
	m = translate(m, vec3(20.0f, -16.7, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.15f, 0.15f, 0.15f));
	room.render(m);

	Program.SendUniform("materialSpecular", 1.0, 1.0, 0.0);
	//Bulb
	m = matrixView;
	m = translate(m, vec3(-2.95f, 4.24f, -1.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(10, 32, 32);



	//Program.SendUniform("materialSpecular", 1.0, 1.0, 0.0);
	//// Get Attribute Locations
	//GLuint attribVertex = Program.GetAttribLocation("aVertex");
	//GLuint attribNormal = Program.GetAttribLocation("aNormal");

	//// Enable vertex attribute arrays
	//glEnableVertexAttribArray(attribVertex);
	//glEnableVertexAttribArray(attribNormal);

	//// Bind (activate) the vertex buffer and set the pointer to it
	//glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	//glVertexAttribPointer(attribVertex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//// Bind (activate) the normal buffer and set the pointer to it
	//glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
	//glVertexAttribPointer(attribNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//// Draw triangles – using index buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	//glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);

	//// Disable arrays
	//glDisableVertexAttribArray(attribVertex);
	//glDisableVertexAttribArray(attribNormal);


	//LIGHTS

	//Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
	//Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);

	//Program.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	//Program.SendUniform("lightDir.diffuse", 0.9, 0.9, 0.9);	  // dimmed white light

	Program.SendUniform("lightPoint.position", -2.95, 4.24, -1.0);
	//Program.SendUniform("lightPoint.diffuse", 0.5, 0.5, 0.5);
	Program.SendUniform("lightPoint.specular", 1.0, 1.0, 1.0);
	Program.SendUniform("Shininess", 3.0f);
	


	Program.SendUniform("matrixModelView", m);
}

void onRender()
{


	// this global variable controls the animation
	float time = glutGet(GLUT_ELAPSED_TIME) * 0.001f;

	// clear screen and buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// setup the View Matrix (camera)
	mat4 m = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));// switch tilt off
	m = translate(m, cam);												// animate camera motion (controlled by WASD keys)
	m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));			// switch tilt on
	m = m * matrixView;
	m = rotate(m, radians(angleRot), vec3(0.f, 1.f, 0.f));				// animate camera orbiting
	matrixView = m;

	// render the scene objects
	renderScene(matrixView, time);

	// essential for double-buffering technique
	glutSwapBuffers();

	// proceed the animation
	glutPostRedisplay();
}

// called before window opened or resized - to setup the Projection Matrix
void onReshape(int w, int h)
{
	float ratio = w * 1.0f / h;      // we hope that h is not zero
	glViewport(0, 0, w, h);
	mat4 matrixProjection = perspective(radians(60.f), ratio, 0.02f, 1000.f);

	// Setup the Projection Matrix
	Program.SendUniform("matrixProjection", matrixProjection);
}

// Handle WASDQE keys
void onKeyDown(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w': cam.z = std::max(cam.z * 1.05f, 0.01f); break;
	case 's': cam.z = std::min(cam.z * 1.05f, -0.01f); break;
	case 'a': cam.x = std::max(cam.x * 1.05f, 0.01f); angleRot = 0.1f; break;
	case 'd': cam.x = std::min(cam.x * 1.05f, -0.01f); angleRot = -0.1f; break;
	case 'e': cam.y = std::max(cam.y * 1.05f, 0.01f); break;
	case 'q': cam.y = std::min(cam.y * 1.05f, -0.01f); break;
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
	// stop orbiting
	if ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) == 0) angleRot = 0;
}

// Handle WASDQE keys (key up)
void onKeyUp(unsigned char key, int x, int y)
{
	switch (tolower(key))
	{
	case 'w':
	case 's': cam.z = 0; break;
	case 'a':
	case 'd': cam.x = 0; break;
	case 'q':
	case 'e': cam.y = 0; break;
	}
}

// Handle arrow keys and Alt+F4
void onSpecDown(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_F4:		if ((glutGetModifiers() & GLUT_ACTIVE_ALT) != 0) exit(0); break;
	case GLUT_KEY_UP:		onKeyDown('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyDown('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyDown('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyDown('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyDown('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyDown('e', x, y); break;
	case GLUT_KEY_F11:		glutFullScreenToggle();
	}
}

// Handle arrow keys (key up)
void onSpecUp(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:		onKeyUp('w', x, y); break;
	case GLUT_KEY_DOWN:		onKeyUp('s', x, y); break;
	case GLUT_KEY_LEFT:		onKeyUp('a', x, y); break;
	case GLUT_KEY_RIGHT:	onKeyUp('d', x, y); break;
	case GLUT_KEY_PAGE_UP:	onKeyUp('q', x, y); break;
	case GLUT_KEY_PAGE_DOWN:onKeyUp('e', x, y); break;
	}
}

// Handle mouse click
bool bJustClicked = false;
void onMouse(int button, int state, int x, int y)
{
	bJustClicked = (state == GLUT_DOWN);
	glutSetCursor(bJustClicked ? GLUT_CURSOR_CROSSHAIR : GLUT_CURSOR_INHERIT);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);
}

// handle mouse move
void onMotion(int x, int y)
{
	if (bJustClicked)
		bJustClicked = false;
	else
	{
		glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH) / 2, glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// find delta (change to) pan & tilt
		float deltaPan = 0.25f * (x - glutGet(GLUT_WINDOW_WIDTH) / 2);
		float deltaTilt = 0.25f * (y - glutGet(GLUT_WINDOW_HEIGHT) / 2);

		// View = Tilt * DeltaPan * Tilt^-1 * DeltaTilt * View;
		angleTilt += deltaTilt;
		mat4 m = mat4(1.f);
		m = rotate(m, radians(angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaPan), vec3(0.f, 1.f, 0.f));
		m = rotate(m, radians(-angleTilt), vec3(1.f, 0.f, 0.f));
		m = rotate(m, radians(deltaTilt), vec3(1.f, 0.f, 0.f));
		matrixView = m * matrixView;
	}
}

int main(int argc, char **argv)
{
	// init GLUT and create Window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("CI5520 3D Graphics Programming");

	// init glew
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		cerr << "GLEW Error: " << glewGetErrorString(err) << endl;
		return 0;
	}
	cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;
	
	// register callbacks
	glutDisplayFunc(onRender);
	glutReshapeFunc(onReshape);
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecDown);
	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);

	cout << "Vendor: " << glGetString(GL_VENDOR) << endl;
	cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
	cout << "Version: " << glGetString(GL_VERSION) << endl;
	

	// init light and everything – not a GLUT or callback function!
	if (!init())
	{
		cerr << "Application failed to initialise" << endl;
		return 0;
	}


	// enter GLUT event processing cycle
	glutMainLoop();

	done();

	return 1;
}

