#ifndef __IMPORTER_MATERIAL_H__
#define __IMPORTER_MATERIAL_H__

#include "Importer.h"

struct aiScene;
struct aiMesh;
class ResourceMaterial;
class Resource;

//struct ImportMaterialData : public ImportData
//{
//	const aiScene* scene = nullptr;
//	const aiMesh* mesh = nullptr;
//	ResourceMaterial* new_material = nullptr;
//};

class ImporterMaterial : public Importer
{

public:
	ImporterMaterial();
	virtual ~ImporterMaterial();

	Resource* Import(const char* path) const override;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Material; };
};

#endif
