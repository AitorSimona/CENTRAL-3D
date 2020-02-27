#ifndef __IMPORTERSCRIPT_H__
#define __IMPORTERSCRIPT_H__

#include "Importer.h"
BE_BEGIN_NAMESPACE
class BROKEN_API ImporterScript : public Importer {
public:
	ImporterScript();
	~ImporterScript();

	virtual Resource* Import(ImportData& IData) const override;
	virtual Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Script; };
};
BE_END_NAMESPACE
#endif // !__IMPORTERSCRIPT_H__

