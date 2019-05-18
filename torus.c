#include "torus.h"

void drawTorusVBO(IndexMesh* mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID)
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), 0);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVBOID);
	glNormalPointer(GL_FLOAT, sizeof(Vec3f), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
	glDrawElements(GL_TRIANGLE_STRIP, ((mesh->rows)*(mesh->cols-1)*2) + ((mesh->cols-1)*2), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void createTorusVB(IndexMesh *mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID)
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f)*(mesh->rows*mesh->cols), mesh->grid, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NormalVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f)*(mesh->rows*mesh->cols), mesh->normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*(((mesh->rows)*(mesh->cols-1)*2) + ((mesh->cols-1)*2)),mesh->indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawTorusVA(IndexMesh* mesh)
{
	glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), mesh->grid);
	glNormalPointer(GL_FLOAT, sizeof(Vec3f), mesh->normals);
	glDrawElements(GL_TRIANGLE_STRIP,((mesh->rows)*(mesh->cols-1)*2) + ((mesh->cols-1)*2), GL_UNSIGNED_INT, mesh->indices);
}

void addTorusIndexVertex(IndexMesh *mesh, int index, float R, float r, float u, float v)
{
	float theta = u * 2.0 * pi;
	float phi = v * 2.0 * pi;

	float x = (R + r*cos(phi)) * cos(theta);
	float y = (R + r*cos(phi)) * sin(theta);
	float z = r * sin(phi);

	float xn = cos(phi) * cos(theta);
	float yn = cos(phi) * sin(theta);
	float zn = sin(phi);

	mesh->grid[index].x = x;
	mesh->grid[index].y = y;
	mesh->grid[index].z = z;

	mesh->normals[index].x = xn;
	mesh->normals[index].y = yn;
	mesh->normals[index].z = zn;
}

void createTorusIndexMesh(IndexMesh *mesh, float majorRadius, float minorRadius, int slices, int stacks)
{
	int i, j;
	float u, v, u1, v1;

	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;

	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	// Allocate a mesh to return.
	mesh->rows = rows;
	mesh->cols = cols;

	allocateIndexMesh(mesh);

	int index = -1;

	/* j outer loop over i inner loop for each circle */
	for (j = 0; j < cols; ++j) {
		v = j / (float)(cols - 1);
		for (i = 0; i < rows; ++i) {
			u = i / (float)(rows - 1);
			addTorusIndexVertex(mesh, ++index, majorRadius, minorRadius, u, v);
		}
	}

	index = -1;

	for (j = 0; j < cols-1; ++j) {
		for (i = 0; i < rows; ++i) {
			mesh->indices[++index] = (((j+1)*rows) + (i%rows));
			mesh->indices[++index] = (((j)*rows) + (i%rows));
		}
		mesh->indices[++index] = (((j+1)*rows) + (i%rows));
		mesh->indices[++index] = (((j+1)*rows) + (i%rows));
	}
}

void addTorusVertex(Mesh *mesh, int index, float R, float r, float u, float v)
{
	float theta = u * 2.0 * pi;
	float phi = v * 2.0 * pi;

	float x = (R + r*cos(phi)) * cos(theta);
	float y = (R + r*cos(phi)) * sin(theta);
	float z = r * sin(phi);

	float xn = cos(phi) * cos(theta);
	float yn = cos(phi) * sin(theta);
	float zn = sin(phi);

	mesh->grid[index].x = x;
	mesh->grid[index].y = y;
	mesh->grid[index].z = z;

	mesh->normals[index].x = xn;
	mesh->normals[index].y = yn;
	mesh->normals[index].z = zn;
}

void createTorusMesh(Mesh *mesh, float majorRadius, float minorRadius, int slices, int stacks)
{
	int i, j;
	float u, v, u1, v1;

	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;

	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	// Allocate a mesh to return.
	mesh->rows = rows;
	mesh->cols = cols;

	allocateMesh(mesh);

	int index = -1;

	/* j outer loop over i inner loop for each circle */
	for (j = 0; j < cols - 1; ++j) {
		v = j / (float)(cols - 1);
		v1 = (j + 1) / (float)(cols - 1);
		for (i = 0; i < rows - 1; ++i) {
			u = i / (float)(rows - 1);
			u1 = (i + 1) / (float)(rows - 1);

			addTorusVertex(mesh, ++index, majorRadius, minorRadius, u, v1);
			addTorusVertex(mesh, ++index, majorRadius, minorRadius, u, v);
			addTorusVertex(mesh, ++index, majorRadius, minorRadius, u1, v1);
			addTorusVertex(mesh, ++index, majorRadius, minorRadius, u1, v);
		}
		addTorusVertex(mesh, ++index, majorRadius, minorRadius, u1, v);
		addTorusVertex(mesh, ++index, majorRadius, minorRadius, u1, v);
		addTorusVertex(mesh, ++index, majorRadius, minorRadius, u1, v1);
		addTorusVertex(mesh, ++index, majorRadius, minorRadius, u1, v1);
	}

#ifdef DEBUG
	printf("number polygons = %d\n", (mesh->rows-1)*(mesh->cols-1)*2);
#endif
}

void drawTorusIM(Mesh* mesh)
{
	int j;

	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = mesh->rows;
	int cols = mesh->cols;

	/* j outer loop over i inner loop for each circle */
	glBegin(GL_TRIANGLE_STRIP);
	for (j = 0; j < ((cols - 1)*(rows - 1)*4) + ((cols-1)*4); ++j) {
		Vec3f normal = mesh->normals[j];
		Vec3f vertex = mesh->grid[j];

		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(vertex.x, vertex.y, vertex.z);
	}
	glEnd();
}

void drawTorusVertex(float R, float r, float u, float v)
{
	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * 2.0 * pi;

	float x = (R + r*cos(phi)) * cos(theta);
	float y = (R + r*cos(phi)) * sin(theta);
	float z = r * sin(phi);

	float xn = cos(phi) * cos(theta);
	float yn = cos(phi) * sin(theta);
	float zn = sin(phi);

	glNormal3f(xn, yn, zn);
	glVertex3f(x, y, z);
}

void drawTorusNM(float majorRadius, float minorRadius, int slices, int stacks)
{
	int i, j;
	float u, v, u1, v1;

	/* Grid divisions (dimensions of quads, "fences") */
	int divsU = slices;
	int divsV = stacks;

	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = divsU + 1;
	int cols = divsV + 1;

	/* j outer loop over i inner loop for each circle */
	glBegin(GL_TRIANGLE_STRIP);
	for (j = 0; j < cols - 1; ++j) {
		v = j / (float)(cols - 1);
		v1 = (j + 1) / (float)(cols - 1);
		for (i = 0; i < rows - 1; ++i) {
			u = i / (float)(rows - 1);
			u1 = (i + 1) / (float)(rows - 1);

			drawTorusVertex(majorRadius, minorRadius, u, v1);
			drawTorusVertex(majorRadius, minorRadius, u, v);
			drawTorusVertex(majorRadius, minorRadius, u1, v1);
			drawTorusVertex(majorRadius, minorRadius, u1, v);
		}
		drawTorusVertex(majorRadius, minorRadius, u1, v);
		drawTorusVertex(majorRadius, minorRadius, u1, v);
		drawTorusVertex(majorRadius, minorRadius, u1, v1);
		drawTorusVertex(majorRadius, minorRadius, u1, v1);
	}
	glEnd();
}
