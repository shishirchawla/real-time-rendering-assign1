#ifdef _WIN32
#include <windows.h>
#endif

/* sdl-base contains opengl/SDL init code and provides
 * a main loop. this file implements expected callback
 * functions. see sdl-base.h */
#include "sdlbase.h"

#include "util.h"
#include "sphere.h"
#include "torus.h"

#define DEBUG

/* Render modes */
enum RenderMode {
	NORMAL_MODE = 0,
	IMMEDIATE_MODE,
	VERTEX_ARRAY_MODE,
	VERTEX_BUFFER_OBJECT_MODE,
	NUM_RENDER_MODES
} renMode = NORMAL_MODE;

enum Shape {
	SPHERE_S = 0,
	TORUS_S,
	NUM_SHAPES
} shape = SPHERE_S;

/* Scene globals */
Camera camera;
float currentFramerate;
float currentFrametime;
int windowWidth;
int windowHeight;
int lastMouseX = 0;
int lastMouseY = 0;

/* Intialize external variable renderOpetions. */
bool renderOptions[NUM_RENDER_OPTIONS] = { false, true, true, false, false, false };

/* Meshes */
Mesh sphereMesh, torusMesh;
IndexMesh sphereIndexMesh, torusIndexMesh;

int shapeTess = 1; //tesselation (default 1x)
float shapeRotation = 0; // rotation
int numShapes = 1; //(default 1x1)

int numLights = 1; //(default on z-axis)
int numPolygons = 0;

/* VBO IDS */
int sphereVertexVBOID, sphereNormalVBOID, sphereIndexVBOID;
int torusVertexVBOID, torusNormalVBOID, torusIndexVBOID;

/* draw axes */
void drawAxes(float len, int lineWidth) {
	if (!renderOptions[RENDER_DRAW_AXES])
		return;
	glPushAttrib(GL_CURRENT_BIT | GL_LINE_BIT);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0); glVertex3f(0,0,0); glVertex3f(len, 0, 0);
	glColor3f(0, 1, 0); glVertex3f(0,0,0); glVertex3f(0, len, 0);
	glColor3f(0, 0, 1); glVertex3f(0,0,0); glVertex3f(0, 0, len);
	glEnd();
	glPopAttrib();
}

void enableVertexArrays(void)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
}

void disableVertexArrays(void)
{
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
}

/* Update opengl state to match flags in renderOptions */
void setRenderOptions()
{
	if (renderOptions[RENDER_LIGHTING])
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	if (renderOptions[RENDER_CULL_BACKFACE])
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	glShadeModel(renderOptions[RENDER_FLAT] ? GL_FLAT : GL_SMOOTH);

	glPolygonMode(GL_FRONT_AND_BACK, 
			renderOptions[RENDER_WIREFRAME] ? GL_LINE : GL_FILL);
}

void freeMesh(Mesh* mesh)
{
	if (mesh->grid)
		free(mesh->grid);
	if (mesh->normals)
		free(mesh->normals);

	mesh->grid = NULL;
	mesh->normals = NULL;
	mesh->rows = 0;
	mesh->cols = 0;
}

void freeIndexMesh(IndexMesh* mesh)
{
	if (mesh->grid)
		free(mesh->grid);
	if (mesh->normals)
		free(mesh->normals);
	if (mesh->indices)
		free(mesh->indices);

	mesh->grid = NULL;
	mesh->normals = NULL;
	mesh->indices = NULL;
	mesh->rows = 0;
	mesh->cols = 0;
}

void createGeometry(void)
{
	//free meshes
	freeMesh(&sphereMesh);
	freeIndexMesh(&sphereIndexMesh);
	freeMesh(&torusMesh);
	freeIndexMesh(&torusIndexMesh);

	//initialize sphere meshes
	createSphereMesh(&sphereMesh, 8 * shapeTess, 4 * shapeTess, 1.0f); 
	createSphereIndexMesh(&sphereIndexMesh, 8 * shapeTess, 4 * shapeTess, 1.0f); 
	createSphereVB(&sphereIndexMesh, sphereVertexVBOID, sphereNormalVBOID, sphereIndexVBOID);	
	//initialize torus meshes
	createTorusMesh(&torusMesh, 0.80f, 0.20f, 8 * shapeTess, 4 * shapeTess); 
	createTorusIndexMesh(&torusIndexMesh, 0.80f, 0.20f, 8 * shapeTess, 4 * shapeTess); 
	createTorusVB(&torusIndexMesh, torusVertexVBOID, torusNormalVBOID, torusIndexVBOID);

	//set number of polygons (average) -- may be a few more with dummy vertices
	numPolygons = (sphereMesh.rows-1)*(sphereMesh.cols-1)*2;
}

/* Called once at program start */
void init()
{
	int argc = 0;  /* fake glutInit args */
	char *argv = "";
	glutInit(&argc, &argv);

	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);

	memset(&renderOptions, 0, sizeof(renderOptions));
	renderOptions[RENDER_LIGHTING] = true;

	memset(&camera, 0, sizeof(Camera));
	camera.sensitivity = 0.3f;
	camera.zoom = 2.0f;

	enableVertexArrays();
	/* Generate VBO IDS */
	glGenBuffers(1, &sphereVertexVBOID);
	glGenBuffers(1, &sphereNormalVBOID);
	glGenBuffers(1, &sphereIndexVBOID);
	glGenBuffers(1, &torusVertexVBOID);
	glGenBuffers(1, &torusNormalVBOID);
	glGenBuffers(1, &torusIndexVBOID);

	createGeometry();
}

/* Called once at start and again on window resize */
void reshape(int width, int height)
{
	windowWidth = width;
	windowHeight = height;

	/* Portion of viewport to render to */
	glViewport(0, 0, width, height);

	/* Calc aspect ratio */
	float aspect = width / (float)height;

	/* Begin editing projection matrix */
	glMatrixMode(GL_PROJECTION);

	/* Clear previous projection */
	glLoadIdentity();

	/* Generate perspective projection matrix */
	gluPerspective(75.0f, aspect, clipNear, clipFar);

	/* Restore modelview as current matrix */
	glMatrixMode(GL_MODELVIEW);
}

void drawOSD()
{
	char *bufp;
	char buffer[32];

	/* Backup previous "enable" state */
	glPushAttrib(GL_ENABLE_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	/* Create a temporary orthographic projection, matching
	 * window dimensions, and push it onto the stack */
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, windowWidth, 0, windowHeight, -1, 1);
	glMatrixMode(GL_MODELVIEW);

	/* draw framerate, frametime and tesselation */
	glPushMatrix();
	glLoadIdentity(); /* clear current modelview (ie. from display) */
	snprintf(buffer, sizeof(buffer), "FR(fps): %d", (int)currentFramerate);
	glRasterPos2i(10, 10);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "FT(ms): %.2f", (float)currentFrametime);
	glRasterPos2i(10, 25);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "Tesselation : %dx", (int)shapeTess);
	glRasterPos2i(10, 40);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "Render Mode: %d", (int)renMode);
	glRasterPos2i(240, 10);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "Grid: %d X %d", (int)numShapes, (int)numShapes);
	glRasterPos2i(240, 25);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "Shape: %d", shape);
	glRasterPos2i(240, 40);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "Number of Polygons: %d", numPolygons);
	glRasterPos2i(470, 10);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	snprintf(buffer, sizeof(buffer), "Number of Lights: %d", numLights);
	glRasterPos2i(470, 25);
	for (bufp = buffer; *bufp; bufp++)
		glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *bufp);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();  /* pop projection */
	glMatrixMode(GL_MODELVIEW);

	/* Restore "enable" state */
	glPopAttrib();
}

void setupCamera(void)
{
	glTranslatef(0.0f, 0.0f, -camera.zoom*numShapes);

	glRotatef(camera.rotX, 1.0f, 0.0f, 0.0f);
	glRotatef(camera.rotY, 0.0f, 1.0f, 0.0f);
}

void setupLighting(void)
{
	static float lightPosition0[] = {0.0f, 0.0f, 3.0f, 1.0f};
	static float lightDirection1[] = {1.0f, 1.0f, 0.0f, 0.0f};
	static float lightDirection2[] = {-1.0f, 1.0f, 0.0f, 0.0f};
	static float lightDirection3[] = {1.0f, -1.0f, 0.0f, 0.0f};
	static float lightDirection4[] = {-1.0f, -1.0f, 0.0f, 0.0f};
	static float lightDirection5[] = {1.0f, 0.0f, 0.0f, 0.0f};
	static float lightDirection6[] = {-1.0f, 0.0f, 0.0f, 0.0f};
	static float lightDirection7[] = {0.0f, 0.0f, -1.0f, 0.0f};

	static float lightAmbient[] = {0.1f, 0.1f, 0.1f, 1.0f}; /* Constant, add to everything */
	static float lightDiffuse[] = {0.3f, 0.5f, 0.9f, 1.0f}; /* Constant, add to everything */

	static float materialDiffuse[] = {0.7f, 0.7f, 0.7f, 1.0f}; /* Brighter as surface faces light */
	static float materialSpecular[] = {0.3f, 0.3f, 0.3f, 1.0f}; /* Highlight, direct reflection from light */
	static float materialShininess = 64.0f; /* 1 to 128, higher gives sharper highlight */

	setRenderOptions();
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);
	glLightfv(GL_LIGHT1, GL_POSITION, lightDirection1);
	glLightfv(GL_LIGHT2, GL_POSITION, lightDirection2);
	glLightfv(GL_LIGHT3, GL_POSITION, lightDirection3);
	glLightfv(GL_LIGHT4, GL_POSITION, lightDirection4);
	glLightfv(GL_LIGHT5, GL_POSITION, lightDirection5);
	glLightfv(GL_LIGHT6, GL_POSITION, lightDirection6);
	glLightfv(GL_LIGHT7, GL_POSITION, lightDirection7);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT2, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT3, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT4, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT4, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT5, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT5, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT6, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT6, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT7, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT7, GL_DIFFUSE, lightDiffuse);

	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
}

void drawSphere(void)
{
	switch(renMode) {
		case NORMAL_MODE :
			drawSphereNM(1.00f, 8*shapeTess, 4*shapeTess);
			break;
		case IMMEDIATE_MODE :
			drawSphereIM(&sphereMesh);
			break;
		case VERTEX_ARRAY_MODE :
			drawSphereVA(&sphereIndexMesh);
			break;
		case VERTEX_BUFFER_OBJECT_MODE :
			drawSphereVBO(&sphereIndexMesh, sphereVertexVBOID, sphereNormalVBOID, sphereIndexVBOID);
			break;
		default :
			break;
	}
}

void drawTorus(void)
{
	switch(renMode) {
		case NORMAL_MODE :
			drawTorusNM(0.80f, 0.20f, 8*shapeTess, 4*shapeTess);
			break;
		case IMMEDIATE_MODE :
			drawTorusIM(&torusMesh);
			break;
		case VERTEX_ARRAY_MODE :
			drawTorusVA(&torusIndexMesh);
			break;
		case VERTEX_BUFFER_OBJECT_MODE :
			drawTorusVBO(&torusIndexMesh, torusVertexVBOID, torusNormalVBOID, torusIndexVBOID);
			break;
		default :
			break;
	}
}

void drawShape(void)
{
	switch (shape) {
		case SPHERE_S :
			drawSphere();
			break;
		case TORUS_S :
			drawTorus();
			break;
		default :
			break;
	}
}

void drawScene(void)
{
	int i, j;

	float startPosX = (((float)numShapes)-1.00f) * -1;
	float startPosY = (((float)numShapes)-1.00f) * -1;
	for (i = 0; i < numShapes; i++) {
		for (j = 0; j < numShapes; j++) {
			glPushMatrix();
			// apply transformation
			glTranslatef(startPosX+(2*j), startPosY+(2*i), 0);
			glRotatef(shapeRotation, 0.0f, 1.0f, 0.0f);
			// draw axes
			if (renderOptions[RENDER_DRAW_AXES])
				drawAxes(1.0f, 2);
			// draw shape
			drawShape();
			glPopMatrix();
		}
	}
}

void display()
{	
	/* Clear the colour and depth buffer */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/* Load Identity */
	glLoadIdentity();
	/* Camera transformations */
	setupCamera();
	/* Lighting */
	setupLighting();
	/* Draw scene */
	drawScene();
	/* OSD - framerate etc */
	drawOSD();
}

void animate(float dt)
{
	float speed = 25.0;

	shapeRotation += dt * speed;
	if (shapeRotation > THREESIXTY)
		shapeRotation -= THREESIXTY;
}

/* Called continuously. dt is time between frames in seconds */
void update(float dt)
{
	static float fpsTime = 0.0f;
	static int fpsFrames = 0;
	fpsTime += dt;
	fpsFrames += 1;
	if (fpsTime > 1.0f)
	{
		currentFramerate = fpsFrames / fpsTime;
		currentFrametime = (1 / currentFramerate) * 1000;
		fpsTime = 0.0f;
		fpsFrames = 0;

	}

	// animate (rotate shapes)
	if (renderOptions[RENDER_ANIMATION])
		animate(dt);

}

void mouseDown(int button, int state, int x, int y)
{
	if (button == SDL_BUTTON_LEFT)
		camera.rotating = (state == 1);
	if (button == SDL_BUTTON_RIGHT)
		camera.zooming = (state == 1);
}

void mouseMove(int x, int y)
{
	int dx = x - lastMouseX;
	int dy = y - lastMouseY;
	if (camera.rotating) {
		camera.rotY += dx * camera.sensitivity;
		camera.rotX += dy * camera.sensitivity;
	}
	if (camera.zooming) {
		camera.zoom -= dy * camera.zoom * camera.sensitivity * 0.03f;
	}
	lastMouseX = x;
	lastMouseY = y;

}

void keyDown(int key)
{
	if (key == SDLK_ESCAPE)
		quit();
	if (key == SDLK_F3)
		renderOptions[RENDER_LIGHTING] = !renderOptions[RENDER_LIGHTING];
	if (key == SDLK_F4)
		renderOptions[RENDER_WIREFRAME] = !renderOptions[RENDER_WIREFRAME];
	if (key == SDLK_F5)
		renderOptions[RENDER_FLAT] = !renderOptions[RENDER_FLAT];
	// switch shape
	if (key == SDLK_s)
		shape = (shape + 1) % (NUM_SHAPES);
	// culling
	if (key == SDLK_c)
		renderOptions[RENDER_CULL_BACKFACE] = !renderOptions[RENDER_CULL_BACKFACE];
	// switch render mode 
	if (key == SDLK_m)
		renMode = (renMode + 1) % (NUM_RENDER_MODES);
	// increase lights
	if (key == SDLK_l)
	{
		numLights = (numLights+1) % maxLights;

		glDisable(GL_LIGHT0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glDisable(GL_LIGHT3);
		glDisable(GL_LIGHT4);
		glDisable(GL_LIGHT5);
		glDisable(GL_LIGHT6);
		glDisable(GL_LIGHT7);
		//enable lights
		switch (numLights) {
			case 8:
				glEnable(GL_LIGHT7);
			case 7:
				glEnable(GL_LIGHT6);
			case 6:
				glEnable(GL_LIGHT5);
			case 5:
				glEnable(GL_LIGHT4);
			case 4:
				glEnable(GL_LIGHT3);
			case 3:
				glEnable(GL_LIGHT2);
			case 2:
				glEnable(GL_LIGHT1);
			case 1:
				glEnable(GL_LIGHT0);
				break;
			case 0:
				glDisable(GL_LIGHTING);
				break;
			default:
				;
		}
	}
	// change number of shapes to draw
	if (key == SDLK_b)
		numShapes *= 2;
	if (key == SDLK_v)
		if (numShapes >= 2)
			numShapes /= 2;
	// double tesselation
	if (key == SDLK_EQUALS)
	{
		shapeTess *= 2;
		//shapeTess += 50;
		createGeometry();
	}
	// half tesselation
	if (key == SDLK_MINUS)
		if (shapeTess >= 2) {
			shapeTess /= 2;
			createGeometry();
		}
	// draw axes
	if (key == SDLK_a)
		renderOptions[RENDER_DRAW_AXES] = !renderOptions[RENDER_DRAW_AXES];
	// rotate shape
	if (key == SDLK_g)
		renderOptions[RENDER_ANIMATION] = !renderOptions[RENDER_ANIMATION];
}

void keyUp(int key)
{
}

void cleanup()
{
	/* free meshes */
	freeMesh(&sphereMesh);
	freeIndexMesh(&sphereIndexMesh);
	freeMesh(&torusMesh);
	freeIndexMesh(&torusIndexMesh);
	/* deallocate buffers */
	glDeleteBuffers(1, &sphereVertexVBOID); glDeleteBuffers(1, &sphereNormalVBOID); glDeleteBuffers(1, &sphereIndexVBOID);
	glDeleteBuffers(1, &torusVertexVBOID); glDeleteBuffers(1, &torusNormalVBOID); glDeleteBuffers(1, &torusIndexVBOID);
}
