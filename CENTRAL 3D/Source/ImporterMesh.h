#ifndef __IMPORTER_MESH_H__
#define __IMPORTER_MESH_H__

#include "Importer.h"

struct aiMesh;
class ComponentMesh;
class ResourceMesh;

struct ImportMeshData : public ImportData
{
	aiMesh* mesh = nullptr;
	ResourceMesh* new_mesh = nullptr;
};

class ImporterMesh : public Importer
{

public:
	ImporterMesh();
	virtual ~ImporterMesh();

	bool Import(const ImportData& IData) const override;

	void Save(ResourceMesh* mesh, const char* path) const;
	void Load(const char* filename, ResourceMesh& mesh) const;
};

#endif
