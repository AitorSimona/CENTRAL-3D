#ifndef __IMPORTER_FOLDER_H__
#define __IMPORTER_FOLDER_H__

#include "Importer.h"

class ResourceFolder;

class ImporterFolder : public Importer
{

public:
	ImporterFolder();
	~ImporterFolder();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void Save(ResourceFolder* folder) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Folder; };
};

#endif
