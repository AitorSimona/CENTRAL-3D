#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"

class Resource;

class Importer
{
public:
	enum class ImporterType
	{
		Scene,
		Model,
		Material,
		Shader,
		Mesh,
		Texture,
		Shader_Object,
		Unknown
	};
public:
	Importer(ImporterType type);
	virtual ~Importer();

	virtual Resource* Import(const char* path) const = 0;
	virtual Resource* Load(const char* path) const = 0;	

	ImporterType GetType() const;

private:
	ImporterType type = ImporterType::Unknown;
};

#endif
