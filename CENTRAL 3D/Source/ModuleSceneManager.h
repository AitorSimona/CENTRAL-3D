#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "Module.h"
#include <vector>
#include "Math.h"
#include "Color.h"
#include "Quadtree.h"

class GameObject;
struct aiScene;
struct ImportMaterialData;
struct par_shapes_mesh_s;
class ResourceMesh;
class ResourceScene;
struct Event;

class ModuleSceneManager : public Module
{
	friend class ModuleResourceManager;
public:

	ModuleSceneManager(bool start_enabled = true);
	~ModuleSceneManager();

	bool Init(json file) override;
	bool Start() override;
	//void ONEvent(const Event& event) const override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	bool CleanUp() override;
	void DrawScene();

	// --- Creators ---
	GameObject* CreateEmptyGameObject();
	GameObject* CreateEmptyGameObjectGivenUID(uint UID);

	//MYTODO: Move all resource stuff to RESOURCE MANAGER
	GameObject* LoadSphere();
	GameObject* LoadCube();
	GameObject* LoadCapsule();
	GameObject* LoadPlane();
	GameObject* LoadCylinder();
	GameObject* LoadDisk();

	void DestroyGameObject(GameObject* go);

	// --- Getters ---
	GameObject* GetSelectedGameObject() const;
	GameObject* GetRootGO() const;

	// --- Setters ---
	void SetSelectedGameObject(GameObject* go);

	// --- Utilities ---
	void RedoOctree();
	void SetStatic(GameObject* go);
	void RecursiveDrawQuadtree(QuadtreeNode * node) const;
	void SelectFromRay(LineSegment& ray);

	// --- Save/Load ----
	void SaveStatus(json &file) const override;
	void LoadStatus(const json & file) override;
	void SaveScene(ResourceScene* scene);
	void SetActiveScene(ResourceScene* scene);

	// --- Primitives ---
	GameObject* LoadPrimitive(uint UID);
	void CreateCapsule(float radius, float height, ResourceMesh* rmesh);
	void CreateCylinder(float radius, float height, ResourceMesh* rmesh);
	void CreateCube(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh);
	void CreateSphere(float Radius, int slices, int slacks, ResourceMesh* rmesh);
	void CreatePlane(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh);
	void CreateDisk(float radius, ResourceMesh* rmesh);

	void SendToDelete(GameObject* go);

private:

	// --- Event Callbacks ---
	static void ONResourceSelected(const Event& e);
	static void ONGameObjectDestroyed(const Event& e);

private:

	GameObject* CreateRootGameObject();
	// --- Primitives ---
	void LoadParMesh(par_shapes_mesh_s* mesh, ResourceMesh* new_mesh) const;
public:
	// --- Actually this is an octree ---
	Quadtree tree;
	bool display_tree = false;
	ResourceScene* currentScene = nullptr;

	// --- Do not modify, just use ---
	ResourceMesh* cube = nullptr;
	ResourceMesh* sphere = nullptr;
	ResourceMesh* capsule = nullptr;
	ResourceMesh* plane = nullptr;
	ResourceMesh* cylinder = nullptr;
	ResourceMesh* disk = nullptr;

	// do not destroy
	ResourceScene* defaultScene = nullptr;
	ResourceScene* temporalScene = nullptr;

private:

	// Game objects to be deleted
	std::vector<GameObject*> go_to_delete;

	uint go_count = 0;
	GameObject* root = nullptr;
	GameObject* SelectedGameObject = nullptr;
};

#endif