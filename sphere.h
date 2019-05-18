#ifndef SPHERE_H
#define SPHERE_H

#include "util.h"

void createSphereMesh(Mesh* mesh, int slices, int stacks, float radius);
void createSphereIndexMesh(IndexMesh *mesh, int slices, int stacks, float radius);
void createSphereVB(IndexMesh *mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID);
void drawSphereNM(float radius, int slices, int stacks);
void drawSphereIM(Mesh* mesh);
void drawSphereVA(IndexMesh* mesh);
void drawSphereVBO(IndexMesh* mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID);

#endif
