#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "Module.h"
#include <vector>
#include "Math.h"
#include "Color.h"
#include "Quadtree.h"

struct aiScene;
struct par_shapes_mesh_s;

BE_BEGIN_NAMESPACE
class GameObject;
struct ImportMaterialData;
class ResourceMesh;
class ResourceScene;
struct Event;

class BROKEN_API ModuleSceneManager : public Module 
{
public:
	friend class ModuleSelection;

	ModuleSceneManager(bool start_enabled = true);
	~ModuleSceneManager();

	bool Init(json& file) override;
	bool Start() override;
	//void ONEvent(const Event& event) const override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	bool CleanUp() override;
	void DrawScene();

	// --- Creators ---
	GameObject* CreateEmptyGameObject();
	GameObject* CreateEmptyGameObjectGivenUID(uint UID);
	void ResetGameObjectUID(GameObject* go);

	//MYTODO: Move all resource stuff to RESOURCE MANAGER
	GameObject* LoadSphere();
	GameObject* LoadCube();
	GameObject* LoadCapsule();
	GameObject* LoadPlane();
	GameObject* LoadCylinder();
	GameObject* LoadDisk();


	void DestroyGameObject(GameObject* go);
	void GatherGameObjects(GameObject* go, std::vector<GameObject*>& gos_vec);

	// --- Getters ---

	// Returns the last selected gameobject
	//GameObject* GetSelectedGameObject() const;
	GameObject* GetRootGO() const;

	// --- Setters ---
	//void SetSelectedGameObject(GameObject* go);

	// --- Utilities ---
	void RedoOctree();
	void RedoOctree(AABB aabb);
	void SetStatic(GameObject* go, bool setStatic, bool setChildren);
	void RecursiveDrawQuadtree(QuadtreeNode* node) const;
	//bool IsSelected(GameObject* go);
	void SelectFromRay(LineSegment& ray);

	// --- Save/Load ----
	void LoadGame(const json & file);
	void SaveScene(ResourceScene* scene);
	void SetActiveScene(ResourceScene* scene);

	// --- Primitives ---
	GameObject* LoadPrimitiveObject(uint PrimitiveMeshID);
	void CreateCapsule(float radius, float height, ResourceMesh* rmesh);
	void CreateCylinder(float radius, float height, ResourceMesh* rmesh);
	void CreateCube(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh);
	void CreateSphere(float Radius, int slices, int slacks, ResourceMesh* rmesh);
	void CreatePlane(float sizeX, float sizeY, float sizeZ, ResourceMesh* rmesh);
	void CreateDisk(float radius, ResourceMesh* rmesh);

	const ResourceMesh* GetCubeMesh()const { return cube; }
	const ResourceMesh* GetSphereMesh() const { return sphere; }
	const ResourceMesh* GetCapsuleMesh() const { return capsule; }
	const ResourceMesh* GetPlaneMesh() const { return plane; }
	const ResourceMesh* GetCylinderMesh() const { return cylinder; }
	const ResourceMesh* GetDiskMesh() const { return disk; }


	void SendToDelete(GameObject* go);

private:

	// --- Event Callbacks ---
	static void ONResourceSelected(const Event& e);
	static void ONGameObjectDestroyed(const Event& e);

private:
	GameObject* CreateRootGameObject();

	// --- Primitives ---
	//For Broken Engine's sake, please put the bool to false if not cube or plane
	void LoadParMesh(par_shapes_mesh_s* mesh, ResourceMesh* new_mesh, bool calculateTangents) const;
	void CalculateTangentAndBitangent(ResourceMesh* mesh, uint index, float3& tangent, float3& bitangent) const;

public:
	//Components helper, check AddComponent function
	std::vector<int> repeatable_components;

	// --- Actually this is an octree ---
	Quadtree tree;
	bool display_tree = false;

	bool update_tree=false;
	uint treeUpdateTimer = 0;
	ResourceScene* currentScene = nullptr;

	// --- Do not modify, just use ---
	ResourceMesh* cube = nullptr;
	ResourceMesh* capsule = nullptr;
	ResourceMesh* cylinder = nullptr;
	ResourceMesh* disk = nullptr;
	ResourceMesh* plane = nullptr;
	ResourceMesh* sphere = nullptr;

	// do not destroy
	ResourceScene* defaultScene = nullptr;
	ResourceScene* temporalScene = nullptr;
private:

	// Game objects to be deleted
	std::vector<GameObject*> go_to_delete;



	uint go_count = 0;
	GameObject* root = nullptr;
	//GameObject* root_selected = nullptr;
	//GameObject* SelectedGameObject = nullptr;
	GameObject* music = nullptr;
public:

	//std::vector<GameObject*> selected_gameobjects;
};

BE_END_NAMESPACE
#endif
