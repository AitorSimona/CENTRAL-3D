#ifndef __IMPORTER_MODEL_H__
#define __IMPORTER_MODEL_H__

#include "Importer.h"
#include <map>
#include <vector>
#include <string>

struct aiNode;
struct aiScene;

BE_BEGIN_NAMESPACE
class GameObject;
class Resource;
class ResourceMesh;
class ResourceModel;
class ResourceMaterial;

struct BROKEN_API ImportModelData : public Importer::ImportData {
	ImportModelData(const char* path) : Importer::ImportData(path) {};

	ResourceModel* model_overwrite = nullptr;
	bool library_deleted = false;
};

class BROKEN_API ImporterModel : public Importer {

public:
	ImporterModel();
	~ImporterModel();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void InstanceOnCurrentScene(const char* model_path, ResourceModel* model) const;

	void Save(ResourceModel* model, std::vector<GameObject*>& model_gos, const std::string& model_name) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Model; };

private:
	void LoadNodes(const aiNode* node, GameObject* parent, const aiScene* scene, std::vector<GameObject*>& scene_gos, const char* path, std::map<uint, ResourceMesh*>& scene_meshes, std::map<uint, ResourceMaterial*>& scene_mats) const;
	void LoadSceneMeshes(const aiScene* scene, std::map<uint, ResourceMesh*>& scene_meshes, const char* source_file) const;
	void FreeSceneMeshes(std::map<uint, ResourceMesh*>* scene_meshes) const;
	void LoadSceneMaterials(const aiScene* scene, std::map<uint, ResourceMaterial*>& scene_mats, const char* source_file, bool library_deleted) const;
	void FreeSceneMaterials(std::map<uint, ResourceMaterial*>* scene_mats) const;
};
BE_END_NAMESPACE
#endif
