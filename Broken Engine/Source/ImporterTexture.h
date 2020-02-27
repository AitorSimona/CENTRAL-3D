#ifndef __IMPORTER_TEXTURE_H__
#define __IMPORTER_TEXTURE_H__

#include "Importer.h"

BE_BEGIN_NAMESPACE
class Resource;
class ResourceTexture;

class BROKEN_API ImporterTexture : public Importer {

public:
	ImporterTexture();
	~ImporterTexture();

	Resource* Import(ImportData& IData) const override;
	Resource* Load(const char* path) const override;

	void Save(ResourceTexture* texture) const;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::Texture; };
};
BE_END_NAMESPACE
#endif
