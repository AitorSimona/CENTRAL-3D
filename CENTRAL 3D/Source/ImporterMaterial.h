#ifndef __IMPORTER_MATERIAL_H__
#define __IMPORTER_MATERIAL_H__

#include "Importer.h"

struct aiScene;
class ResourceMaterial;

struct ImportMaterialData : public ImportData
{
	const aiScene* scene = nullptr;
	ResourceMaterial* new_material = nullptr;
};

class ImporterMaterial : public Importer
{

public:
	ImporterMaterial();
	virtual ~ImporterMaterial();

	bool Import(const char* File_path, const ImportData& IData) const override;
	void Load(const char* filename, ResourceMaterial& mat);
};

#endif
