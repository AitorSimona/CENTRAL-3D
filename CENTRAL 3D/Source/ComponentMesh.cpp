#include "ComponentMesh.h"
#include "OpenGL.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* ContainerGO) : Component(ContainerGO,Component::ComponentType::Mesh)
{
}

ComponentMesh::~ComponentMesh()
{
	//resource_mesh->instances--;
}

const AABB & ComponentMesh::GetAABB() const
{
		return resource_mesh->aabb;
}

void ComponentMesh::Save(json& file) const
{
	//component_path = MESHES_FOLDER;
//component_path.append(std::to_string(mesh->resource_mesh->GetUID()));
//component_path.append(".mesh");

//if(!App->fs->Exists(component_path.data()))
//IMesh->Save(scene_gos[i]->GetComponent<ComponentMesh>()->resource_mesh, component_path.data());

// --- Store path to component file ---
//file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())] = component_path;
}

