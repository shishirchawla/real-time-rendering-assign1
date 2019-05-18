#include "sphere.h"

void createSphereVB(IndexMesh *mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID)
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f)*(mesh->rows*mesh->cols), mesh->grid, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, NormalVBOID);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec3f)*(mesh->rows*mesh->cols), mesh->normals, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*((mesh->rows)*(mesh->cols-1)*2),mesh->indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void addSphereIndexVertex(IndexMesh *mesh, int index, float r, float u, float v)
{
	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = r * cos(theta) * sin(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(phi);

	float xn = x/r;
	float yn = y/r;
	float zn = z/r;

	mesh->grid[index].x = x;
	mesh->grid[index].y = y;
	mesh->grid[index].z = z;

	mesh->normals[index].x = xn;
	mesh->normals[index].y = yn;
	mesh->normals[index].z = zn;
}

void createSphereIndexMesh(IndexMesh *mesh, int slices, int stacks, float radius)
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
			addSphereIndexVertex(mesh, ++index, radius, u, v);
		}
	}

	index = -1;

	for (j = 0; j < cols-1; ++j) {
		for (i = 0; i < rows; ++i) {
			mesh->indices[++index] = (((j)*rows) + (i%rows));
			mesh->indices[++index] = (((j+1)*rows) + (i%rows));
		}
	}
}

void addSphereVertex(Mesh *mesh, int index, float r, float u, float v)
{
	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = r * cos(theta) * sin(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(phi);

	float xn = x/r;
	float yn = y/r;
	float zn = z/r;

	Vec3f vertex = {x, y, z};
	Vec3f normal = {xn, yn, zn};

	mesh->grid[index].x = x;
	mesh->grid[index].y = y;
	mesh->grid[index].z = z;

	mesh->normals[index].x = xn;
	mesh->normals[index].y = yn;
	mesh->normals[index].z = zn;
}

void createSphereMesh(Mesh *mesh, int slices, int stacks, float radius)
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

			addSphereVertex(mesh, ++index, radius, u, v);
			addSphereVertex(mesh, ++index, radius, u, v1);
			addSphereVertex(mesh, ++index, radius, u1, v);
			addSphereVertex(mesh, ++index, radius, u1, v1);
		}
	}
}

void drawSphereIM(Mesh* mesh)
{
	int j;

	/* Vertex dimensions ("posts"). Note for a sphere, the
	 * last vertex of each row is the same as the first */
	int rows = mesh->rows;
	int cols = mesh->cols;

	/* j outer loop over i inner loop for each circle */
	glBegin(GL_TRIANGLE_STRIP);
	for (j = 0; j < (cols - 1)*(rows - 1)*4; ++j) {
		Vec3f normal = mesh->normals[j];
		Vec3f vertex = mesh->grid[j];

		glNormal3f(normal.x, normal.y, normal.z);
		glVertex3f(vertex.x, vertex.y, vertex.z);
	}
	glEnd();
}

void drawSphereVA(IndexMesh* mesh)
{
	glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), mesh->grid);
	glNormalPointer(GL_FLOAT, sizeof(Vec3f), mesh->normals);
	glDrawElements(GL_TRIANGLE_STRIP,((mesh->rows)*(mesh->cols-1)*2), GL_UNSIGNED_INT, mesh->indices);
}

void drawSphereVBO(IndexMesh* mesh, int VertexVBOID, int NormalVBOID, int IndexVBOID)
{
	glBindBuffer(GL_ARRAY_BUFFER, VertexVBOID);
	glVertexPointer(3, GL_FLOAT, sizeof(Vec3f), 0);
	glBindBuffer(GL_ARRAY_BUFFER, NormalVBOID);
	glNormalPointer(GL_FLOAT, sizeof(Vec3f), 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBOID);
	glDrawElements(GL_TRIANGLE_STRIP, ((mesh->rows)*(mesh->cols-1)*2), GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawSphereVertex(float r, float u, float v)
{
	/* Use maths rather than physics spherical coordinate convention */
	float theta = u * 2.0 * pi;
	float phi = v * pi;

	float x = r * cos(theta) * sin(phi);
	float y = r * sin(theta) * sin(phi);
	float z = r * cos(phi);

	/* NOTE: a sphere is the ONLY geometry for which the normal
	 * is always in the same direction as the vertex.
	 * this is a unit sphere so they are even the same values. */
	glNormal3f(x/r, y/r, z/r);

	glVertex3f(x, y, z);
}

void drawSphereNM(float radius, int slices, int stacks)
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
			drawSphereVertex(radius, u, v);
			drawSphereVertex(radius, u, v1);
			drawSphereVertex(radius, u1, v);
			drawSphereVertex(radius, u1, v1);
		}
	}
	glEnd();
}
