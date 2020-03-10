#ifndef __IMPORTER_MATERIAL_H__
#define __IMPORTER_MATERIAL_H__

#include "Importer.h"

struct aiMaterial;

BE_BEGIN_NAMESPACE
class ResourceMaterial;
class Resource;

struct BROKEN_API ImportMaterialData : public Importer::ImportData {
	ImportMaterialData(const char* path) : Importer::ImportData(path) {};

	const aiMaterial* mat = nullptr;
};

class BROKEN_API ImporterMaterial : public Importer {

public:
	ImporterMaterial();
	virtual ~ImporterMaterial();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void Save(ResourceMaterial* mat) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Material; };
};
BE_END_NAMESPACE
#endif
