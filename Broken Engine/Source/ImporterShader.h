#ifndef __IMPORTER_SHADER_H__
#define __IMPORTER_SHADER_H__

#include "Importer.h"

BE_BEGIN_NAMESPACE
class ResourceShader;
class Resource;


class BROKEN_API ImporterShader : public Importer 
{

public:
	ImporterShader();
	virtual ~ImporterShader();

	Resource* Import(ImportData& IData) const override;

	void Save(ResourceShader* shader) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Shader; };
};
BE_END_NAMESPACE

#endif
