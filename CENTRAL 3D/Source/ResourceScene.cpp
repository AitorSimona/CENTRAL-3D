#include "ResourceScene.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleSceneManager.h"

#include "GameObject.h"

#include "ImporterScene.h"

#include "mmgr/mmgr.h"


ResourceScene::ResourceScene(uint UID, std::string source_file) : Resource(Resource::ResourceType::SCENE, UID, source_file)
{
	extension = ".scene";
	resource_file = source_file;
	original_file = resource_file;

	previewTexID = App->gui->sceneTexID;

}

ResourceScene::~ResourceScene()
{
	NoStaticGameObjects.clear();
	StaticGameObjects.clear();
}

bool ResourceScene::LoadInMemory()
{
	// MYTODO: Clean this

	// --- Load scene game objects ---

	if (NoStaticGameObjects.size() == 0 && App->fs->Exists(resource_file.c_str()))
	{
		// --- Load Scene/model file ---
		json file = App->GetJLoader()->Load(resource_file.c_str());

		// --- Delete buffer data ---
		if (!file.is_null())
		{
			std::vector<GameObject*> objects;

			// --- Iterate main nodes ---
			for (json::iterator it = file.begin(); it != file.end(); ++it)
			{
				// --- Retrieve GO's UID ---
				std::string uid = it.key().c_str();

				// --- Create a Game Object for each node ---
				GameObject* go = App->scene_manager->CreateEmptyGameObjectGivenUID(std::stoi(uid));

				std::string name = file[it.key()]["Name"];
				go->SetName(name.c_str());

				if (!file[it.key()]["Active"].is_null())
					go->GetActive() = file[it.key()]["Active"];

				if (!file[it.key()]["Static"].is_null())
					go->Static = file[it.key()]["Static"];

				if (!file[it.key()]["Index"].is_null())
					go->index = file[it.key()]["Index"];

				if (!file[it.key()]["PrefabChild"].is_null())
					go->is_prefab_child = file[it.key()]["PrefabChild"];

				if (!file[it.key()]["PrefabInstance"].is_null())
					go->is_prefab_instance = file[it.key()]["PrefabInstance"];

				if (!file[it.key()]["Model"].is_null())
					go->model = (ResourceModel*)App->resources->ImportAssets(Importer::ImportData(file[it.key()]["Model"].get<std::string>().c_str()));

				// --- Iterate components ---
				json components = file[it.key()]["Components"];

				for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
				{
					std::string stringuid = it2.key();
					uint uid = std::stoi(stringuid);
					uint type_uint = 0;
					int c_index = -1;
					bool active = false;
	
					// --- Load basic data ---
					if (!components[it2.key()]["Index"].is_null())
						c_index = components[it2.key()]["Index"].get<uint>();

					if (!components[it2.key()]["Active"].is_null())
						active = components[it2.key()]["Active"].get<bool>();

					if (!components[it2.key()]["Type"].is_null())
						type_uint = components[it2.key()]["Type"].get<uint>();

					Component::ComponentType type = (Component::ComponentType)type_uint;
					Component* component = nullptr;

					// --- Create/Retrieve Component ---
					component = go->AddComponent(type, c_index);

					// --- Load Component Data ---
					if (component)
					{
						component->SetUID(uid);
						component->GetActive() = active;
						//component->Load(components[it2.key()]);
					}

				}

				objects.push_back(go);
			}

			App->scene_manager->GetRootGO()->childs.clear();

			// --- Parent Game Objects / Build Hierarchy ---
			for (uint i = 0; i < objects.size(); ++i)
			{
				// --- Call components load ---
				std::vector<Component*>* go_components = &objects[i]->GetComponents();

				for (std::vector<Component*>::const_iterator it = go_components->begin(); it != go_components->end(); ++it)
				{
					(*it)->Load(file[std::to_string(objects[i]->GetUID())]["Components"][std::to_string((*it)->GetUID())]);
				}

				std::string parent_uid_string = file[std::to_string(objects[i]->GetUID())]["Parent"];
				int parent_uid = std::stoi(parent_uid_string);

				if (parent_uid > 0)
				{
					for (uint j = 0; j < objects.size(); ++j) 
					{
						if (parent_uid == objects[j]->GetUID())
						{
							objects[j]->AddChildGO(objects[i], objects[i]->index);
							break;
						}
					}
				}
				else
					App->scene_manager->GetRootGO()->AddChildGO(objects[i], objects[i]->index);
			}
		}
	}

	return true;
}

void ResourceScene::FreeMemory()
{
	// --- Delete all scene game objects ---
	for (std::unordered_map<uint, GameObject*>::iterator it = NoStaticGameObjects.begin(); it != NoStaticGameObjects.end(); ++it)
	{
		delete (*it).second;
	}

	NoStaticGameObjects.clear();

	for (std::unordered_map<uint, GameObject*>::iterator it = StaticGameObjects.begin(); it != StaticGameObjects.end(); ++it)
	{
		delete (*it).second;
	}

	StaticGameObjects.clear();

	// Note that this will be called once we load another scene, and the octree will be cleared right after this 
}

void ResourceScene::CreateInspectorNode()
{
}

void ResourceScene::DeactivateAllGameObjects()
{
	for (std::unordered_map<uint, GameObject*>::iterator it = NoStaticGameObjects.begin(); it != NoStaticGameObjects.end(); ++it)
	{
		(*it).second->GetActive() = false;
	}

	for (std::unordered_map<uint, GameObject*>::iterator it = StaticGameObjects.begin(); it != StaticGameObjects.end(); ++it)
	{
		(*it).second->GetActive() = false;
	}
}

void ResourceScene::ActivateAllGameObjects()
{
	for (std::unordered_map<uint, GameObject*>::iterator it = NoStaticGameObjects.begin(); it != NoStaticGameObjects.end(); ++it)
	{
		(*it).second->GetActive() = true;
	}

	for (std::unordered_map<uint, GameObject*>::iterator it = StaticGameObjects.begin(); it != StaticGameObjects.end(); ++it)
	{
		(*it).second->GetActive() = false;
	}
}

// Created for on-play temporal scene 
void ResourceScene::CopyInto(ResourceScene* target)
{
	if (target)
	{
		// --- Delete all scene game objects ---
		for (std::unordered_map<uint, GameObject*>::iterator it = NoStaticGameObjects.begin(); it != NoStaticGameObjects.end(); ++it)
		{
			target->NoStaticGameObjects[(*it).first] = (*it).second;
		}

		for (std::unordered_map<uint, GameObject*>::iterator it = StaticGameObjects.begin(); it != StaticGameObjects.end(); ++it)
		{
			target->StaticGameObjects[(*it).first] = (*it).second;
		}
	}
}

GameObject* ResourceScene::GetGOWithName(const char* GO_name)
{
	GameObject* ret = nullptr;

	std::string GO_stringname = GO_name;

	for (std::unordered_map<uint, GameObject*>::iterator it = NoStaticGameObjects.begin(); it != NoStaticGameObjects.end(); ++it)
	{
		std::string name = (*it).second->GetName();
		if (name.compare(GO_stringname) == 0)
		{
			ret = (*it).second;
			return ret;
		}
	}

	for (std::unordered_map<uint, GameObject*>::iterator it = StaticGameObjects.begin(); it != StaticGameObjects.end(); ++it)
	{
		std::string name = (*it).second->GetName();
		if (name.compare(GO_stringname) == 0)
		{
			ret = (*it).second;
			return ret;
		}
	}

	return ret;
}

//Return the GameObject with this UID
GameObject* ResourceScene::GetGOWithUID(uint UID)
{
	GameObject* ret_go = nullptr;

	std::unordered_map<uint, GameObject*>::iterator it;
	it = NoStaticGameObjects.find(UID);

	if (it != NoStaticGameObjects.end())
		ret_go = it->second;

	if (ret_go == nullptr)
	{
		it = StaticGameObjects.find(UID);
		if (it != StaticGameObjects.end())
			ret_go = it->second;
	}

	return ret_go;
}

void ResourceScene::OnOverwrite()
{
	// No reason to overwrite scene
}

void ResourceScene::OnDelete()
{
	if (this->GetUID() == App->scene_manager->currentScene->GetUID()
		&& (this->GetUID() != App->scene_manager->defaultScene->GetUID()))
	{
		App->scene_manager->SetActiveScene(App->scene_manager->defaultScene);
	}

	FreeMemory();

	if (this->GetUID() == App->scene_manager->defaultScene->GetUID())
	{
		// --- Reset octree ---
		App->scene_manager->tree.SetBoundaries(AABB(float3(-100, -100, -100), float3(100, 100, 100)));
	
		// --- Release current scene ---
		App->scene_manager->currentScene->Release();
	
		// --- Clear root ---
		App->scene_manager->GetRootGO()->childs.clear();
	}

	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);

	if (this->GetUID() != App->scene_manager->defaultScene->GetUID())
		App->resources->ONResourceDestroyed(this);
	else
		LoadToMemory();
}

