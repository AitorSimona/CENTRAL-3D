#ifndef __RESOURCE_MESH_H__
#define __RESOURCE_MESH_H__

#include "Resource.h"

#include "glew\include\GL\glew.h"

struct aiMesh;

struct Vertex
{
	GLfloat position[3];
	GLfloat normal[3];
	GLubyte color[4];
	GLfloat texCoord[2];
};

class ResourceMesh : public Resource
{
public:

	ResourceMesh();
	virtual ~ResourceMesh();

	void ImportMesh(aiMesh* mesh);

	void GenerateVBO();
	void GenerateIBO();
	void GenerateVAO();

public:

	Vertex* Vertices = nullptr;
	uint VerticesSize = 0;

	GLuint* Indices = nullptr;
	uint IndicesSize = 0;

	GLuint VBO = 0;
	GLuint IBO = 0;
	GLuint VAO = 0;
};

#endif

