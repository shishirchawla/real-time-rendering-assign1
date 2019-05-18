#ifndef TORUS_H
#define TORUS_H

#include "util.h"

void createTorusMesh(Mesh *mesh, float majorRadius, float minorRadius, int slices, int stacks);
void createTorusIndexMesh(IndexMesh *mesh, float majorRadius, float minorRadius, int slices, int stacks);
void createTorusVB(IndexMesh *mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID);
void drawTorusNM(float majorRadius, float minorRadius, int slices, int stacks);
void drawTorusIM(Mesh* mesh);
void drawTorusVA(IndexMesh* mesh);
void drawTorusVBO(IndexMesh* mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID);

#endif
