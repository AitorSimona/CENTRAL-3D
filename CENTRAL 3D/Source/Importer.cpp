#include "Importer.h"

Importer::Importer(ImporterType type)
{
	this->type = type;
}

Importer::~Importer()
{
}

Importer::ImporterType Importer::GetType() const
{
	return type;
}

