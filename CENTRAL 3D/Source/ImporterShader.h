#ifndef __IMPORTER_SHADER_H__
#define __IMPORTER_SHADER_H__

#include "Importer.h"

class ResourceShader;

struct ImportShaderData : public ImportData
{
	const char * vertexPath;
	const char * fragmentPath;
};

class ImporterShader : public Importer
{

public:
	ImporterShader();
	virtual ~ImporterShader();

	bool Import(const ImportData& IData) const override;

	void Save(ResourceShader* shader, const char* path) const;
	void Load(const char* filename, ResourceShader& shader) const;
};

#endif
