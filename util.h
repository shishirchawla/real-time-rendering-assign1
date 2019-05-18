#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>
//#include <GL/gl.h>
//#include <GL/glut.h>
#include <GLUT/glut.h>

#define THREESIXTY 360.0

/* Vector struct - this should be moved to another file,
 * perhaps with utility add/sub/dot/normalize functions. */
typedef struct {
  float x, y, z;
} Vec3f;

typedef struct {
	Vec3f* grid;
	Vec3f* normals;
	int rows;
	int cols;
} Mesh;

typedef struct {
	Vec3f* grid;
	Vec3f* normals;
	unsigned int* indices;
	int rows;
	int cols;
} IndexMesh;

/* Basic camera struct */
typedef struct {
  int rotating;
  int zooming;
  float rotX, rotY;
  float zoom;
  float sensitivity;
} Camera;

/* Render state enums */
enum RenderOptions {
  RENDER_LIGHTING,
  RENDER_WIREFRAME,
  RENDER_FLAT,
  RENDER_CULL_BACKFACE,
  RENDER_DRAW_AXES,
  RENDER_ANIMATION,
  NUM_RENDER_OPTIONS /* MUST BE LAST! */
};


static const float pi = 3.14159265f;

static const float clipFar = 1000.00f;
static const float clipNear = 0.01f;

/* Max light modes -- 0 for no lighting */
static const int maxLights = 9;

extern bool renderOptions[NUM_RENDER_OPTIONS];

void allocateMesh(Mesh *mesh);
void allocateIndexMesh(IndexMesh *mesh);

/** Util functions */
void *safe_malloc(size_t size);

#endif
