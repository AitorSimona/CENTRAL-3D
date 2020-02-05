#include "ComponentMesh.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleFileSystem.h"

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
	if (resource_mesh)
		return resource_mesh->aabb;
	else
		return AABB();
}

json ComponentMesh::Save() const
{
	json node;

	// --- Store path to component file ---
	node["Resources"]["ResourceMesh"] = std::string(resource_mesh->GetResourceFile());

	return node;
}

void ComponentMesh::Load(json& node)
{
	std::string path = node["Resources"]["ResourceMesh"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	resource_mesh = (ResourceMesh*)App->resources->GetResource(std::stoi(path));
}

