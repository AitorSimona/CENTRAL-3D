#ifndef __IMPORTER_MODEL_H__
#define __IMPORTER_MODEL_H__

#include "Importer.h"
#include <map>
#include <vector>
#include <string>

struct aiNode;
struct aiScene;
struct aiMesh;

BE_BEGIN_NAMESPACE
class GameObject;
class Resource;
class ResourceMesh;
class ResourceBone;
class ResourceAnimation;
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

	GameObject* InstanceOnCurrentScene(const char* model_path, ResourceModel* model) const;

	void Save(ResourceModel* model, std::vector<GameObject*>& model_gos, const std::string& model_name) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Model; };

private:
	void LoadNodes(const aiNode* node, GameObject* parent, const aiScene* scene, std::vector<GameObject*>& scene_gos, const char* path, std::map<uint, ResourceMesh*>& scene_meshes, std::map<uint, ResourceMaterial*>& scene_mats, std::vector<aiMesh*> &mesh_collector, std::vector<GameObject*>& mesh_wbones) const;
	void LoadSceneMeshes(const aiScene* scene, std::map<uint, ResourceMesh*>& scene_meshes, const char* source_file) const;
	void FreeSceneMeshes(std::map<uint, ResourceMesh*>* scene_meshes) const;
	void LoadSceneMaterials(const aiScene* scene, std::map<uint, ResourceMaterial*>& scene_mats, const char* source_file, bool library_deleted) const;
	void LoadSceneBones(std::vector<aiMesh*>& mesh, std::vector<GameObject*>& mesh_wbones, std::map<uint, ResourceBone*>& bones, const char* source_file) const;
	void FreeSceneBones(std::map<uint, ResourceBone*>* scene_bones) const;
	void LoadBones(std::vector<GameObject*> model_gos, std::vector<aiMesh*> mesh_collector, std::map<uint, ResourceBone*> bones) const;
	void LoadSceneAnimations(const aiScene* scene, GameObject* GO, std::map<uint, ResourceAnimation*>& anim, const char* source_file) const;
	void FreeSceneAnimations(std::map<uint, ResourceAnimation*>* anims) const;
	void FreeSceneMaterials(std::map<uint, ResourceMaterial*>* scene_mats) const;
};
BE_END_NAMESPACE
#endif
