#include "ResourceScene.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleSceneManager.h"

#include "GameObject.h"

#include "ImporterScene.h"

#include "mmgr/mmgr.h"

namespace BrokenEngine {
	ResourceScene::ResourceScene(uint UID, std::string source_file) : Resource(Resource::ResourceType::SCENE, UID, source_file) {
		extension = ".scene";
		name = "scene";
		resource_file = SCENES_FOLDER + name + extension;

		previewTexID = App->gui->sceneTexID;

	}

	ResourceScene::~ResourceScene() {
	}

	bool ResourceScene::LoadInMemory() {
		// --- Load scene game objects ---

		if (scene_gos.size() == 0) {
			// --- Load Scene/model file ---
			json file = App->GetJLoader()->Load(resource_file.c_str());

			// --- Delete buffer data ---
			if (!file.is_null()) {
				std::vector<GameObject*> objects;

				// --- Iterate main nodes ---
				for (json::iterator it = file.begin(); it != file.end(); ++it) {
					// --- Create a Game Object for each node ---
					GameObject* go = App->scene_manager->CreateEmptyGameObject();

					// --- Retrieve GO's UID and name ---
					go->SetName(it.key().c_str());
					std::string uid = file[it.key()]["UID"];
					go->GetUID() = std::stoi(uid);

					// --- Iterate components ---
					json components = file[it.key()]["Components"];


					for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2) {
						// --- Determine ComponentType ---
						std::string type_string = it2.key();
						uint type_uint = std::stoi(type_string);
						Component::ComponentType type = (Component::ComponentType)type_uint;

						Component* component = nullptr;

						// --- Create/Retrieve Component ---
						component = go->AddComponent(type);

						// --- Load Component Data ---
						if (component)
							component->Load(components[type_string]);

					}

					objects.push_back(go);
				}


				// --- Parent Game Objects / Build Hierarchy ---
				for (uint i = 0; i < objects.size(); ++i) {
					std::string parent_uid_string = file[objects[i]->GetName()]["Parent"];
					uint parent_uid = std::stoi(parent_uid_string);

					for (uint j = 0; j < objects.size(); ++j) {
						if (parent_uid == objects[j]->GetUID()) {
							objects[j]->AddChildGO(objects[i]);
							break;
						}
					}
				}
			}
		}

		return true;
	}

	void ResourceScene::FreeMemory() {
		// --- Delete all scene game objects ---
		for (std::unordered_map<uint, GameObject*>::iterator it = scene_gos.begin(); it != scene_gos.end(); ++it) {
			delete (*it).second;
		}

		scene_gos.clear();
	}

	void ResourceScene::OnOverwrite() {
		// No reason to overwrite scene
	}

	void ResourceScene::OnDelete() {
		FreeMemory();
		App->fs->Remove(resource_file.c_str());

		App->resources->RemoveResourceFromFolder(this);
		App->resources->ONResourceDestroyed(this);
	}
}
