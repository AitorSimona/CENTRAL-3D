#ifndef __IMPORTER_FOLDER_H__
#define __IMPORTER_FOLDER_H__

#include "Importer.h"
BE_BEGIN_NAMESPACE
class ResourceFolder;

class BROKEN_API ImporterFolder : public Importer {

public:
	ImporterFolder();
	~ImporterFolder();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void Save(ResourceFolder* folder) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Folder; };
};
BE_END_NAMESPACE
#endif
