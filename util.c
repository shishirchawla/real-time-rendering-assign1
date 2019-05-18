#include "util.h"

void allocateMesh(Mesh *mesh)
{
	mesh->grid = (Vec3f*)safe_malloc(sizeof(Vec3f) * (((mesh->rows-1) * (mesh->cols-1) * 4) + ((mesh->cols-1) * 4)));
	mesh->normals = (Vec3f*)safe_malloc(sizeof(Vec3f) * (((mesh->rows-1) * (mesh->cols-1) * 4) + ((mesh->cols-1) * 4)));
}

void allocateIndexMesh(IndexMesh *mesh)
{
	mesh->grid = (Vec3f*)safe_malloc(sizeof(Vec3f) * (mesh->rows) * (mesh->cols));
	mesh->normals = (Vec3f*)safe_malloc(sizeof(Vec3f) * (mesh->rows) * (mesh->cols));
	mesh->indices = (unsigned int*)safe_malloc(sizeof(unsigned int) * (((mesh->rows) * (mesh->cols-1) * 2) + ((mesh->cols-1) * 2)));
}

void *safe_malloc(size_t size)
{
	void *mem_block = NULL;
	if ((mem_block = calloc(1, size)) == NULL) {
		fprintf(stderr, "ERROR - calloc()");
		exit(EXIT_FAILURE);
	}

	return mem_block;
}
