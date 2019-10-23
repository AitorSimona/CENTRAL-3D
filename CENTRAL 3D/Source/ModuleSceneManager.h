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
	GameObject* CreateCube(float sizeX, float sizeY, float sizeZ, bool checkers = false);
	GameObject* CreateSphere(float Radius, int slices, int slacks, bool checkers = false);
	void CreateGrid() const;

	// --- Getters ---
	uint GetNumGameObjects() const;
	uint GetSelectedGameObjects();
	std::vector<GameObject*>& GetGameObjects();

	// --- Setters ---
	void SetSelectedGameObject(uint index);
	void SetTextureToSelectedGO(uint id);

	// --- Utilities ---
	void Draw() const;

public:
	ComponentMaterial* CheckersMaterial = nullptr;
	ComponentMaterial* DefaultMaterial = nullptr;

private:
	uint SelectedGameObject = 0;
	std::vector<GameObject*> game_objects;
	std::vector<ComponentMaterial*> Materials;
};

#endif