#ifndef __IMPORTER_TEXTURE_H__
#define __IMPORTER_TEXTURE_H__

#include "Importer.h"

namespace BrokenEngine {
	class Resource;
	class ResourceTexture;

	class ImporterTexture : public Importer {

	public:
		ImporterTexture();
		~ImporterTexture();

		Resource* Import(ImportData& IData) const override;
		Resource* Load(const char* path) const override;

		void Save(ResourceTexture* texture) const;

		static inline Importer::ImporterType GetType() { return Importer::ImporterType::Texture; };
	};
}

#endif
