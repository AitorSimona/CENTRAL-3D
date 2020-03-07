#ifndef __IMPORTER_SCENE_H__
#define __IMPORTER_SCENE_H__

#include "Importer.h"
#include <vector>
#include <string>

class GameObject;
class Resource;
class ResourceScene;


class ImporterScene : public Importer
{
	
public:
	ImporterScene();
	virtual ~ImporterScene();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;
	void SaveSceneToFile(ResourceScene* scene) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Scene; };};

#endif
