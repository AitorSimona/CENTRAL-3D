#ifndef __IMPORTER_PREFAB_H__
#define __IMPORTER_PREFAB_H__

#include "Importer.h"

BE_BEGIN_NAMESPACE

class Resource;
class ResourcePrefab;

class BROKEN_API ImporterPrefab : public Importer
{

public:
	ImporterPrefab();
	~ImporterPrefab();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void Save(ResourcePrefab* prefab) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Prefab; };
};
BE_END_NAMESPACE

#endif
