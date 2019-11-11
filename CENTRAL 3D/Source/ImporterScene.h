#ifndef __IMPORTER_SCENE_H__
#define __IMPORTER_SCENE_H__

#include "Importer.h"

class ImporterMesh;
class ImporterMaterial;
struct aiNode;
struct aiScene;
class ComponentMaterial;
class GameObject;

struct ImportSceneData : public ImportData
{

};

class ImporterScene : public Importer
{

public:
	ImporterScene();
	virtual ~ImporterScene();

	bool Import(const char* File_path, const ImportData& IData) const override;

private:
	void LoadNodes(const aiNode* node, GameObject* parent ,const aiScene* scene, ComponentMaterial* Material) const;
	ImporterMesh* IMesh = nullptr;
	ImporterMaterial* IMaterial = nullptr;
};

#endif
