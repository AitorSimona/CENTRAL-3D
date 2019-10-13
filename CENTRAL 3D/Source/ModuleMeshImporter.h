#ifndef __MODULE_MESH_IMPORTER_H__
#define __MODULE_MESH_IMPORTER_H__

#include <vector>
#include "Module.h"
#include "Math.h"

class ComponentMesh;
class aiScene;
class GameObject;

// Specifying normal vectors length (used when drawing normals)
#define NORMAL_LENGTH 1

class ModuleMeshImporter : public Module
{
public:
	// --- Basic ---
	ModuleMeshImporter(bool start_enabled = true);
	~ModuleMeshImporter();
	bool Init(json file);
	bool Start();
	bool CleanUp();

	// --- Getters ---
	uint GetNumMeshes() const;
	void GetTextureIDFromSceneMaterial(const aiScene& scene, uint & texture_ID, std::string & directory);

	// --- Utilities ---
	void Draw() const;
	bool LoadFBX(const char* path);

private:
	// --- Draw Functiions accessed by main Draw (which is called by renderer) ---
	void DrawMesh(const ComponentMesh* mesh) const;
	void DrawNormals(const ComponentMesh* mesh) const;

private:
	std::vector<GameObject*> game_objects;
};

#endif
