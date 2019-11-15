#include "ResourceMesh.h"
#include "OpenGL.h"

#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh() : Resource(Resource::ResourceType::MESH)
{
}

ResourceMesh::~ResourceMesh()
{
	glDeleteBuffers(1, (GLuint*)&VerticesID);
	glDeleteBuffers(1, (GLuint*)&IndicesID);
	glDeleteBuffers(1, (GLuint*)&TextureCoordsID);

	if (Vertices)
	{
		delete[] Vertices;
		Vertices = nullptr;
	}
	if (Indices)
	{
		delete[] Indices;
		Indices = nullptr;
	}
	if (Normals)
	{
		delete[] Normals;
		Normals = nullptr;
	}
	if (TexCoords)
	{
		delete[] TexCoords;
		TexCoords = nullptr;
	}
}

void ResourceMesh::CreateAABB()
{
	aabb.SetNegativeInfinity();
	aabb.Enclose(Vertices, VerticesSize);
}

void ResourceMesh::LoadOnMemory()
{
}

void ResourceMesh::FreeMemory()
{
}
