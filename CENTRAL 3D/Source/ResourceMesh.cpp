#include "ResourceMesh.h"

ResourceMesh::ResourceMesh() : Resource(Resource::ResourceType::MESH)
{
}

ResourceMesh::~ResourceMesh()
{
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
