#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "Module.h"
#include <vector>

class GameObject;
class ComponentMaterial;
struct aiScene;
struct ImportMaterialData;

class ModuleSceneManager : public Module
{
public:

	ModuleSceneManager(bool start_enabled = true);
	~ModuleSceneManager();

	bool Init(json file);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	// --- Creators ---
	GameObject* CreateEmptyGameObject();
	ComponentMaterial* CreateEmptyMaterial();

	// --- Getters ---
	uint GetNumGameObjects() const;

	// --- Utilities ---
	void Draw() const;

private:

	std::vector<GameObject*> game_objects;
	std::vector<ComponentMaterial*> Materials;
};

#endif