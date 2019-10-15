#include "ModuleSceneManager.h"
#include "OpenGL.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ComponentMaterial.h"
#include "ModuleRenderer3D.h"

#include "mmgr/mmgr.h"

ModuleSceneManager::ModuleSceneManager(bool start_enabled)
{
}

ModuleSceneManager::~ModuleSceneManager()
{
}

bool ModuleSceneManager::Init(json file)
{
	return true;
}

bool ModuleSceneManager::Start()
{
	return true;
}

update_status ModuleSceneManager::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSceneManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleSceneManager::CleanUp()
{
	// --- Clean Up object data ---
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		if (game_objects[i])
			delete game_objects[i];
	}

	for (uint i = 0; i < Materials.size(); ++i)
	{
		if (Materials[i])
			delete Materials[i];
	}

	return true;
}

GameObject * ModuleSceneManager::CreateEmptyGameObject()
{
	// --- Create empty Game object to be filled out ---
	GameObject* new_object = new GameObject("GO"); // MYTODO: We should create name like GameObject, GameObject1 ...
	game_objects.push_back(new_object);

	return new_object;
}

ComponentMaterial * ModuleSceneManager::CreateEmptyMaterial()
{
	// --- Creating Empty material to be filled out ---
	ComponentMaterial* Material = new ComponentMaterial(Component::ComponentType::Material);

	if (Material)
		Materials.push_back(Material);

	return Material;
}

ComponentMaterial * ModuleSceneManager::CreateMaterialFromScene(const aiScene& scene, const char& file_path)
{
	// --- Creating material from scene data, note that we are not including assimp here since we are only passing aiscene by parameter to another function ---
	ComponentMaterial* Material = new ComponentMaterial(Component::ComponentType::Material);
	Material->CreateFromScene(scene, file_path);

	if (Material)
		Materials.push_back(Material);

	return Material;
}

uint ModuleSceneManager::GetNumGameObjects() const
{
	return game_objects.size();
}

void ModuleSceneManager::Draw() const
{

	// --- Activate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// --- Draw Game Object Meshes ---
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		ComponentRenderer* Renderer = (ComponentRenderer*)game_objects[i]->GetComponent(Component::ComponentType::Renderer);

		if (Renderer)
		{
			Renderer->Draw();
		}
		else
		{
			//LOG("|[error]: Could not find Renderer component in current game object");
		}
	}

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
}
