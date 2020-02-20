#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "Globals.h"

class Resource;

class Importer
{
public:
	enum class ImporterType
	{
		Folder,
		Scene,
		Model,
		Material,
		Shader,
		Mesh,
		Texture,
		Shader_Object,
		Meta,
		Unknown
	};

	struct ImportData
	{
		ImportData(const char* path)
		{
			this->path = path;
		}

		const char* path = "";
	};

public:
	Importer(ImporterType type);
	virtual ~Importer();

	virtual Resource* Import(ImportData& IData) const = 0;
	virtual Resource* Load(const char* path) const = 0;	

	ImporterType GetType() const;

private:
	ImporterType type = ImporterType::Unknown;
};

#endif
