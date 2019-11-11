#ifndef __IMPORTER_MATERIAL_H__
#define __IMPORTER_MATERIAL_H__

#include "Importer.h"

struct aiScene;
class ComponentMaterial;

struct ImportMaterialData : public ImportData
{
	const aiScene* scene = nullptr;
	ComponentMaterial* new_material = nullptr;
};

class ImporterMaterial : public Importer
{

public:
	ImporterMaterial();
	virtual ~ImporterMaterial();

	bool Import(const char* File_path, const ImportData& IData) const override;
};

#endif
