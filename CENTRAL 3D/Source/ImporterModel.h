#ifndef __IMPORTER_MODEL_H__
#define __IMPORTER_MODEL_H__

#include "Importer.h"

class ImporterModel : public Importer
{

public:
	ImporterModel();
	~ImporterModel();

    Resource* Import(const char* path) const override;
    Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Model; };
};

#endif
