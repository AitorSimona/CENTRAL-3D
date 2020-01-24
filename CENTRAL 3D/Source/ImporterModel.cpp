#include "ImporterModel.h"

ImporterModel::ImporterModel() : Importer(Importer::ImporterType::Model)
{
}

ImporterModel::~ImporterModel()
{
}

// --- Import external file ---
Resource* ImporterModel::Import(const char* path) const
{
	return nullptr;
}

// --- Load file from library ---
Resource* ImporterModel::Load(const char* path) const
{
	return nullptr;
}