#ifndef __IMPORTER_FONT_H__
#define __IMPORTER_FONT_H__

#include "Importer.h"

BE_BEGIN_NAMESPACE

class Resource;

class BROKEN_API ImporterFont : public Importer
{

public:
	ImporterFont();
	virtual ~ImporterFont();

	Resource* Import(ImportData& IData) const override;
	void Save(ImporterFont* font) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Font; };

};


BE_END_NAMESPACE
#endif