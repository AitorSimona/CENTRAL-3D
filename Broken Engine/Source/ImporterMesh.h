#ifndef __IMPORTER_MESH_H__
#define __IMPORTER_MESH_H__

#include "Importer.h"

struct aiMesh;

BE_BEGIN_NAMESPACE
class ResourceMesh;
class Resource;
class ResourceMesh;

struct BROKEN_API ImportMeshData : public Importer::ImportData {
	ImportMeshData(const char* path) : Importer::ImportData(path) {};

	aiMesh* mesh = nullptr;
};

class BROKEN_API ImporterMesh : public Importer {

public:
	ImporterMesh();
	virtual ~ImporterMesh();

	Resource* Import(ImportData& IData) const override;

	void Save(ResourceMesh* mesh) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Mesh; };
};
BE_END_NAMESPACE
#endif
