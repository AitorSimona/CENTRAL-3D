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

				// --- Iterate components ---
				json components = file[it.key()]["Components"];
				

				for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
				{
					// --- Determine ComponentType ---
					std::string type_string = it2.key();
					uint type_uint = std::stoi(type_string);
					Component::ComponentType type = (Component::ComponentType)type_uint;

					// --- and index ---
					int c_index = -1;
					json index = components[it2.key()]["index"];

					if (!index.is_null())
						c_index = index.get<uint>();

					Component* component = nullptr;

					// --- Create/Retrieve Component ---
					component = go->AddComponent(type, c_index);

					// --- Load Component Data ---
					if (component)
						component->Load(components[type_string]);

				}

				objects.push_back(go);
			}


			// --- Parent Game Objects / Build Hierarchy ---
			for (uint i = 0; i < objects.size(); ++i)
			{
				std::string parent_uid_string = file[std::to_string(objects[i]->GetUID())]["Parent"];
				uint parent_uid = std::stoi(parent_uid_string);

				for (uint j = 0; j < objects.size(); ++j)
				{
					if (parent_uid == objects[j]->GetUID())
					{
						objects[j]->AddChildGO(objects[i]);
						break;
					}
				}
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
	if (ret_go == nullptr) {
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
	if (this->GetUID() == App->scene_manager->defaultScene->GetUID())
	{
		if (this->GetUID() == App->scene_manager->currentScene->GetUID())
		{
			App->scene_manager->SetActiveScene(App->scene_manager->defaultScene);
		}

		FreeMemory();
		App->fs->Remove(resource_file.c_str());

		App->resources->RemoveResourceFromFolder(this);
		App->resources->ONResourceDestroyed(this);
	}

}

