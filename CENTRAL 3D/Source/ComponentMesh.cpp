#include "ComponentMesh.h"
#include "OpenGL.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* ContainerGO) : Component(ContainerGO,Component::ComponentType::Mesh)
{
}

ComponentMesh::~ComponentMesh()
{
	resource_mesh->instances--;
}

const AABB & ComponentMesh::GetAABB() const
{
		return resource_mesh->aabb;
}

