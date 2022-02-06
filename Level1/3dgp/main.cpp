//SPOTLIGHT FAIL version
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


bool lightOn0, lightOn1, lightOn2, lightOn3, lightOn4;

// 3D models
C3dglModel camera;
C3dglModel table;
C3dglModel vase;
C3dglModel teapot;
C3dglModel helmet;
C3dglModel room;
C3dglModel lamp0;
C3dglModel lamp1;
C3dglModel ceillamp;

C3dglProgram Program;

C3dglBitmap bm;
C3dglBitmap bm2; 
GLuint idTexWood;
GLuint idTexNone;
GLuint idCloth;

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

	// load your 3D models here!
	if (!camera.load("models\\camera.3ds")) return false;
	if (!table.load("models\\table.obj")) return false;
	if (!vase.load("models\\vase.obj")) return false;
	if (!teapot.load("models\\teapot.obj")) return false;
	if (!helmet.load("models\\Mandalorian.obj")) return false;
	if (!room.load("models\\LivingRoomObj\\LivingRoom.obj")) return false;
	if (!lamp0.load("models\\lamp.obj")) return false;
	if (!lamp1.load("models\\lamp.obj")) return false;
	if (!ceillamp.load("models\\ceilinglamp.3ds")) return false;

	room.loadMaterials("models\\LivingRoomObj\\");

	// Initialise the View Matrix (initial position of the camera)
	matrixView = rotate(mat4(1.f), radians(angleTilt), vec3(1.f, 0.f, 0.f));
	matrixView *= lookAt(
		vec3(0.0, 18.0, 25.0),
		vec3(0.0, 18.0, 0.0),
		vec3(0.0, 1.0, 0.0));

	//TEXTURES
	bm.Load("textures/oak.bmp", GL_RGBA);
	if (!bm.GetBits()) return false;
	bm2.Load("textures/cloth.bmp", GL_RGBA);
	if (!bm2.GetBits()) return false;

	// Send the texture info to the shaders
	Program.SendUniform("texture0", 0);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &idTexWood);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.GetWidth(), bm.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm.GetBits());

	glGenTextures(1, &idCloth);
	glBindTexture(GL_TEXTURE_2D, idCloth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm2.GetWidth(), bm2.GetHeight(), 0, GL_RGBA,
		GL_UNSIGNED_BYTE, bm2.GetBits());

	// none (simple-white) texture
	glGenTextures(1, &idTexNone);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	BYTE bytes[] = { 255, 255, 255 };
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_BGR, GL_UNSIGNED_BYTE, &bytes);

	//////////////////////   LIGHTS   //////////////////////////

	lightOn0 = false;
	lightOn1 = false;
	lightOn2 = false;
	lightOn3 = false;
	lightOn4 = true;

	//AMBIENT LIGHT
	Program.SendUniform("lightAmbient.color", 0.0, 0.0, 0.0);

	//DIRECTIONAL LIGHT
	Program.SendUniform("lightDir.direction", 1.0, 0.5, 1.0);
	Program.SendUniform("lightDir.diffuse", 0.0, 0.0, 0.0);

	//POINT LIGHT 1
	Program.SendUniform("lightPoint.position", 6.7f, 3.8f, -6.0f);
	Program.SendUniform("lightPoint.diffuse", 0.0, 0.0, 0.0);
	Program.SendUniform("lightPoint.specular", 0.0, 0.0, 0.0);
	Program.SendUniform("shininess", 13.0f);

	////POINT LIGHT 2
	Program.SendUniform("lightPoint2.position", 26.7f, 3.8f, 6.0f);
	Program.SendUniform("lightPoint2.diffuse", 0.0, 0.0, 0.0);
	Program.SendUniform("lightPoint2.specular", 0.0, 0.0, 0.0);
	Program.SendUniform("shininess", 13.0f);

	////SPOT LIGHT 1  (SWINGING/ANIMATED LIGHT)
	Program.SendUniform("spotLight1.position", 20.0f, 13.0f, 0.0f);
	Program.SendUniform("spotLight1.diffuse", 0.5, 0.5, 0.5);
	Program.SendUniform("spotLight1.specular", 0.5, 0.5, 0.5);
	Program.SendUniform("shininess", 13.0f);
	//new uniform values
	Program.SendUniform("spotLight1.direction", 0.0, -1.0, 0.0);
	Program.SendUniform("spotLight1.cutoff", radians(40.0f));
	Program.SendUniform("spotLight1.attenuation", 6);

	//EMISSIVE
	Program.SendUniform("lightEmissive.color", 1.0, 1.0, 1.0);

	// setup the screen background colour
	glClearColor(0.0f, 0.0f, 0.2f, 1.0f);   // dark blue background

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
	Program.SendUniform("materialEmissive", 0.0, 0.0, 0.0); //Avoid Emissive Light from all the objects but the bulbs


	//table
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexWood);
	Program.SendUniform("materialAmbient", 1.0, 1.0, 1.0);
	Program.SendUniform("materialDiffuse", 1.0, 1.0, 1.0);
	Program.SendUniform("materialSpecular", 1.0, 1.0, 1.0);
	m = matrixView;
	m = translate(m, vec3(20.0f, -16.7, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.020f, 0.020f, 0.020f));
	table.render(1, m);


	glBindTexture(GL_TEXTURE_2D, idCloth);
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

	glBindTexture(GL_TEXTURE_2D, idTexNone);


	// vase
	Program.SendUniform("materialAmbient", 0.0, 0.0, 1.0);
	Program.SendUniform("materialDiffuse", 0.0, 0.0, 1.0);
	Program.SendUniform("materialSpecular", 0.0, 0.0, 1.0);
	m = matrixView;
	m = translate(m, vec3(20.0f, -1.5f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.4f, 0.4f, 0.4f));
	vase.render(m);

	// teapot
	Program.SendUniform("materialAmbient", 1.0, 1.0, 0.0);
	Program.SendUniform("materialDiffuse", 1.0, 1.0, 0.0);
	Program.SendUniform("materialSpecular", 1.0, 1.0, 0.0);
	m = matrixView;
	m = translate(m, vec3(15.0f, -1.5f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(2.f, 2.f, 2.f));
	teapot.render(m);

	// helmet
	Program.SendUniform("materialAmbient", 1.0, 0.0, 0.0);
	Program.SendUniform("materialDiffuse", 1.0, 0.0, 0.0);
	Program.SendUniform("materialSpecular", 1.0, 0.0, 0.0);
	m = matrixView;
	m = translate(m, vec3(25.0f, -1.5f, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.15f, 0.15f, 0.15f));
	helmet.render(m);

	//room
	Program.SendUniform("materialAmbient", 0.4, 0.4, 0.4);
	Program.SendUniform("materialDiffuse", 0.4, 0.4, 0.4);
	Program.SendUniform("materialSpecular", 0.0, 0.0, 0.0);
	m = matrixView;
	m = translate(m, vec3(20.0f, -16.7, 0.0f));
	m = rotate(m, radians(180.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.15f, 0.15f, 0.15f));
	room.render(m);

	//lamp
	Program.SendUniform("materialAmbient", 1.0, 0.0, 1.0);
	Program.SendUniform("materialDiffuse", 1.0, 0.0, 1.0);
	Program.SendUniform("materialSpecular", 1.0, 0.0, 1.0);
	glBindTexture(GL_TEXTURE_2D, idTexNone);
	m = matrixView;
	m = translate(m, vec3(10.0f, -1.5f, -6.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.09f, 0.09f, 0.09f));
	lamp0.render(m);

	//Bulb
	Program.SendUniform("materialAmbient", 0.5, 0.5, 0.5);
	Program.SendUniform("materialDiffuse", 0.5, 0.5, 0.5);
	Program.SendUniform("materialSpecular", 0.5, 0.5, 0.5);
	if (lightOn1) 
	{
		Program.SendUniform("materialEmissive", 1.0, 1.0, 1.0);
	}
	else 
	{
		Program.SendUniform("materialEmissive", 0.2, 0.2, 0.2);
	}
	m = matrixView;
	m = translate(m, vec3(6.7f, 3.8f, -6.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(4, 32, 32);
	Program.SendUniform("lightPoint.matrix", m);

	//lamp2
	Program.SendUniform("materialEmissive", 0.0, 0.0, 0.0);
	Program.SendUniform("materialAmbient", 1.0, 0.0, 1.0);
	Program.SendUniform("materialDiffuse", 1.0, 0.0, 1.0);
	Program.SendUniform("materialSpecular", 1.0, 0.0, 1.0);
	m = matrixView;
	m = translate(m, vec3(30.0f, -1.5f, 6.0f));
	m = rotate(m, radians(0.f), vec3(0.0f, 1.0f, 0.0f));
	m = scale(m, vec3(0.09f, 0.09f, 0.09f));
	lamp1.render(m);


	//Bulb2
	Program.SendUniform("materialAmbient", 0.5, 0.5, 0.5);
	Program.SendUniform("materialDiffuse", 0.5, 0.5, 0.5);
	Program.SendUniform("materialSpecular", 0.5, 0.5, 0.5);
	if (lightOn0)
	{
		Program.SendUniform("materialEmissive", 1.0, 1.0, 1.0);
	}
	else
	{
		Program.SendUniform("materialEmissive", 0.2, 0.2, 0.2);
	}
	m = matrixView;
	m = translate(m, vec3(26.7f, 3.8f, 6.0f));
	m = scale(m, vec3(0.1f, 0.1f, 0.1f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(4, 32, 32);
	Program.SendUniform("lightPoint2.matrix", m);


	/////////////////////////////////////

	//ANIMATED LIGHT - SPOT LIGHT

	// Pendulum mechanics
	static float prevT = 0;		// previous timestamp
	static float alpha = 0;		// current pendulum angle
	static float delta = 0.2f;		// angle increase rate (Hooke's law)
	delta -= alpha * (time - prevT) * 0.02;
	alpha += delta;
	prevT = time;
	
	
	// Ceiling lamp
	Program.SendUniform("materialEmissive", 0.0, 0.0, 0.0);
	Program.SendUniform("materialAmbient", 0.66, 0.66, 0.66);
	Program.SendUniform("materialDiffuse", 0.66, 0.66, 0.66);
	Program.SendUniform("materialSpecular", 0.66, 0.66, 0.66);
	m = matrixView;
	m = translate(m, vec3(20, 34, 0));
	m = rotate(m, radians(alpha), vec3(0.5, 0, 1));
	m = translate(m, vec3(-20, -34, 0));
	mat4 m1 = m;
	m = translate(m, vec3(20, 34, 0));
	m = scale(m, vec3(0.2f, 0.2f, 0.2f));
	ceillamp.render(m);

	//object bulb attached to the matrix model view
	Program.SendUniform("materialAmbient", 0.5, 0.5, 0.5);
	Program.SendUniform("materialDiffuse", 0.5, 0.5, 0.5);
	Program.SendUniform("materialSpecular", 0.5, 0.5, 0.5);
	if (lightOn4)
	{
		Program.SendUniform("materialEmissive", 1.0, 1.0, 1.0);
	}
	else
	{
		Program.SendUniform("materialEmissive", 0.2, 0.2, 0.2);
	}
	m = m1;
	m = translate(m, vec3(20.0f, 13.0f, 0.0f));
	m = scale(m, vec3(0.12f, 0.12f, 0.12f));
	Program.SendUniform("matrixModelView", m);
	glutSolidSphere(4, 32, 32);
	// the actual light 
	m = m1;
	m = translate(m, vec3(20.0f, 13.0f, 0.0f));
	m = scale(m, vec3(0.12f, 0.12f, 0.12f));
	Program.SendUniform("spotLight1.matrix", m);

	

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

	Program.SendUniform("matrixModelView", m);
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

	case '1': 
		if (lightOn0) 
		{	
			//light off
			Program.SendUniform("lightPoint2.specular", 0.0, 0.0, 0.0);
			Program.SendUniform("lightPoint2.diffuse", 0.0, 0.0, 0.0);
			lightOn0 = false;
			break;
		}
		else 
		{
			//light on
			Program.SendUniform("lightPoint2.specular", 0.5, 0.5, 0.5);
			Program.SendUniform("lightPoint2.diffuse", 0.5, 0.5, 0.5);
			lightOn0 = true;
			break;
		}

	case '2':
		if (lightOn1)
		{
			//light off
			Program.SendUniform("lightPoint.specular", 0.0, 0.0, 0.0);
			Program.SendUniform("lightPoint.diffuse", 0.0, 0.0, 0.0);

			lightOn1 = false;
			break;
		}
		else
		{
			//light on
			Program.SendUniform("lightPoint.specular", 0.5, 0.5, 0.5);
			Program.SendUniform("lightPoint.diffuse", 0.5, 0.5, 0.5);
			lightOn1 = true;
			break;
		}

	case '3':
		if (lightOn2)
		{
			//light off
			Program.SendUniform("lightAmbient.color", 0.0, 0.0, 0.0);
			lightOn2 = false;
			break;
		}
		else
		{
			//light on
			Program.SendUniform("lightAmbient.color", 0.1, 0.1, 0.1);
			lightOn2 = true;
			break;
		}

	case '4':
		if (lightOn3)
		{
			//light off
			Program.SendUniform("lightDir.diffuse", 0.0, 0.0, 0.0);
			lightOn3 = false;
			break;
		}
		else
		{
			//light on
			Program.SendUniform("lightDir.diffuse", 0.3, 0.3, 0.3);
			lightOn3 = true;
			break;
		}
	case '5':
		if (lightOn4)
		{
			//light off
			Program.SendUniform("spotLight1.specular", 0.0, 0.0, 0.0);
			Program.SendUniform("spotLight1.diffuse", 0.0, 0.0, 0.0);
			lightOn4 = false;
			break;
		}
		else
		{
			//light on
			Program.SendUniform("spotLight1.specular", 0.5, 0.5, 0.5);
			Program.SendUniform("spotLight1.diffuse", 0.5, 0.5, 0.5);
			lightOn4 = true;
			break;
		}
	}
	// speed limit
	cam.x = std::max(-0.15f, std::min(0.15f, cam.x));
	cam.y = std::max(-0.15f, std::min(0.15f, cam.y));
	cam.z = std::max(-0.15f, std::min(0.15f, cam.z));
	// stop orbiting
	if ((glutGetModifiers() & GLUT_ACTIVE_SHIFT) == 0) angleRot = 0;

	//All the lights off? Set emissive light black (grey bulbs) otherwise white.
	if (!lightOn0 && !lightOn1 && !lightOn2 && !lightOn3 && !lightOn4)
	{
		Program.SendUniform("lightEmissive.color", 0.0, 0.0, 0.0);

	}
	else
	{
		Program.SendUniform("lightEmissive.color", 1.0, 1.0, 1.0);
	}
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

