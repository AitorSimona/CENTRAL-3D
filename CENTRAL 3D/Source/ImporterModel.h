#ifndef __IMPORTER_MODEL_H__
#define __IMPORTER_MODEL_H__

#include "Importer.h"
#include <vector>
#include <string>

class GameObject;

class ImporterModel : public Importer
{

public:
	ImporterModel();
	~ImporterModel();

    Resource* Import(ImportData& IData) const override;
    Resource* Load(const char* path) const override;

	void Save(std::vector<GameObject*>& model_gos, std::string& model_name) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Model; };
};

#endif
