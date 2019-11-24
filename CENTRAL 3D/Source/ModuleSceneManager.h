#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__

#include "Module.h"
#include <vector>
#include "Math.h"
#include "Color.h"
#include "Quadtree.h"

class GameObject;
class ComponentMaterial;
struct aiScene;
struct ImportMaterialData;
struct par_shapes_mesh_s;
class ResourceMesh;


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
	void CreateGrid() const;
	GameObject* LoadCube();
	GameObject* LoadSphere();

	void DestroyGameObject(GameObject* go);

	// --- Getters ---
	GameObject* GetSelectedGameObject() const;

	// --- Setters ---
	void SetSelectedGameObject(GameObject* go);
	void SetTextureToSelectedGO(uint id);

	// --- Utilities ---
	void Draw();
	GameObject* GetRootGO() const;
	void RedoOctree();
	void SetStatic(GameObject* go);
	void RecursiveDrawQuadtree(QuadtreeNode * node) const;
	void SelectFromRay(LineSegment& ray);

	// --- Save/Load ----
	void SaveStatus(json &file) const override;
	void LoadStatus(const json & file) override;
	void SaveScene();
	void LoadScene();
	void RecursiveFreeScene(GameObject* go);

	// --- Draw Wireframe using given vertices ---
	template <typename Box>
	static void DrawWire(const Box& box, Color color)
	{
		float3 corners[8];
		box.GetCornerPoints(corners);
		DrawWireFromVertices(corners, color);
	};

private:
	void GatherGameObjects(std::vector<GameObject*> & scene_gos, GameObject* go);
	GameObject* CreateRootGameObject();
	void DrawScene();

	// --- Primitives ---
	void LoadParMesh(par_shapes_mesh_s* mesh, ResourceMesh* new_mesh) const;
	ResourceMesh* CreateCube(float sizeX, float sizeY, float sizeZ);
	ResourceMesh* CreateSphere(float Radius, int slices, int slacks);

	static void DrawWireFromVertices(const float3* corners, Color color);
public:
	ComponentMaterial* CheckersMaterial = nullptr;
	ComponentMaterial* DefaultMaterial = nullptr;

	// --- Actually this is an octree ---
	Quadtree tree;
	std::vector<GameObject*> NoStaticGo;
	bool display_tree = false;
	bool display_boundingboxes = false;
private:
	uint go_count = 0;
	GameObject* root = nullptr;
	GameObject* SelectedGameObject = nullptr;
	std::vector<ComponentMaterial*> Materials;
	std::vector<AABB> aabb;

	ResourceMesh* cube = nullptr;
	ResourceMesh* sphere = nullptr;



};

#endif