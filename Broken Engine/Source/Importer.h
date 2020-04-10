#ifndef __IMPORTER_H__
#define __IMPORTER_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class Resource;

class BROKEN_API Importer {
public:
	enum class ImporterType {
		Folder,
		Scene,
		Model,
		Prefab,
		Material,
		Shader,
		Mesh,
		Bone,
		Animation,
		Animator,
		Texture,
		Shader_Object,
		Script,
		Meta,
		Font,
		Unknown
	};

	struct ImportData {
		ImportData(const char* path) {
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
BE_END_NAMESPACE
#endif
