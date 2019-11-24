#ifndef __IMPORTER_SCENE_H__
#define __IMPORTER_SCENE_H__

#include "Importer.h"
#include <vector>
#include <map>

class ImporterMesh;
class ImporterMaterial;
struct aiNode;
struct aiScene;
class ComponentMaterial;
class GameObject;
class ResourceMesh;

struct ImportSceneData : public ImportData
{

};

enum ExportFileTypes
{
	MODEL,
	SCENE
};

class ImporterScene : public Importer
{

public:
	ImporterScene();
	virtual ~ImporterScene();

	bool Import(const char* File_path, const ImportData& IData) const override;
	bool Load(const char* exported_file) const override;
	std::string SaveSceneToFile(std::vector<GameObject*>& scene_gos, std::string& scene_name, ExportFileTypes exportedfile_type) const;

private:
	void LoadSceneMeshes(const aiScene* scene) const;
	void FreeSceneMeshes() const;
	void LoadNodes(const aiNode* node, GameObject* parent ,const aiScene* scene,  std::vector<GameObject*>& scene_gos, const char* File_path, const char* original_path) const;
	ImporterMesh* IMesh = nullptr;
	ImporterMaterial* IMaterial = nullptr;

	mutable std::map<uint,ResourceMesh*> scene_meshes;
};

#endif
