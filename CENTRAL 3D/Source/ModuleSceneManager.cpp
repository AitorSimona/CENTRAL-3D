#include "ModuleSceneManager.h"
#include "OpenGL.h"
#include "Application.h"
#include "GameObject.h"
#include "ComponentRenderer.h"
#include "ModuleRenderer3D.h"

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
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		if (game_objects[i])
			delete game_objects[i];
	}

	return true;
}

GameObject * ModuleSceneManager::CreateEmptyGameObject()
{
	GameObject* new_object = new GameObject("GO"); // MYTODO: We should create name like GameObject, GameObject1 ...
	game_objects.push_back(new_object);

	return new_object;
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
