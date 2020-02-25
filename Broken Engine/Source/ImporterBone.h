#ifndef __IMPORTER_BONE_H__
#define __IMPORTER_BONE_H__

#include "Importer.h"

#include <map>

struct aiBone;
class ResourceBone;
class Resource;
class GameObject;


struct ImportBoneData : public Importer::ImportData
{
	ImportBoneData(const char* path) : Importer::ImportData(path) {};

	aiBone* bone = nullptr;
};

class ImporterBone : public Importer
{

public:
	ImporterBone();
	virtual ~ImporterBone();

	Resource* Import(ImportData& IData) const override;

	void Save(ResourceBone* bone) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Bone; };

};

#endif