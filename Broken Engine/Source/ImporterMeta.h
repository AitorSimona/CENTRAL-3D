#ifndef __IMPORTER_META_H__
#define __IMPORTER_META_H__

#include "Importer.h"

class ResourceMeta;

class ImporterMeta : public Importer
{

public:
	ImporterMeta();
	~ImporterMeta();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void Save(ResourceMeta* meta) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Meta; };
};

#endif
