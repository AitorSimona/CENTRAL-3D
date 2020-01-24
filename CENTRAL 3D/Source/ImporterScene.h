#ifndef __IMPORTER_SCENE_H__
#define __IMPORTER_SCENE_H__

#include "Importer.h"
#include <vector>
#include <map>
#include <string>

struct aiNode;
struct aiScene;
class ComponentMaterial;
class GameObject;
class ResourceMesh;
class Resource;


class ImporterScene : public Importer
{
	
public:
	ImporterScene();
	virtual ~ImporterScene();

	Resource* Import(const char* path) const override;
	Resource* Load(const char* path) const override;
	std::string SaveSceneToFile(std::vector<GameObject*>& scene_gos, std::string& scene_name) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Scene; };

private:
	void LoadSceneMeshes(const aiScene* scene) const;
	void FreeSceneMeshes() const;
	void LoadNodes(const aiNode* node, GameObject* parent ,const aiScene* scene,  std::vector<GameObject*>& scene_gos, const char* File_path, const char* original_path) const;

	mutable std::map<uint,ResourceMesh*> scene_meshes;
};

#endif
