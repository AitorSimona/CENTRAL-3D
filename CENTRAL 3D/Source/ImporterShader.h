#ifndef __IMPORTER_SHADER_H__
#define __IMPORTER_SHADER_H__

#include "Importer.h"

class ResourceShader;
class Resource;


class ImporterShader : public Importer
{

public:
	ImporterShader();
	virtual ~ImporterShader();

	Resource* Import(const char* path) const override;

	void Save(ResourceShader* shader, const char* path) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Shader; };
};

#endif
