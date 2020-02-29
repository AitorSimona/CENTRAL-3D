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
		Bone,
		Animation,
		Texture,
		Shader_Object,
		Script,
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
		bool dropped = false;
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
