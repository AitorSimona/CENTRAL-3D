#include "Importer.h"

using namespace BrokenEngine;
// MYTODO: passing type may be unneeded
Importer::Importer(ImporterType type) {
	this->type = type;
}

Importer::~Importer() {
}

Importer::ImporterType Importer::GetType() const {
	return type;
}

