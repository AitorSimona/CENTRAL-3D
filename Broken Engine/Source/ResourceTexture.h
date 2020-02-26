#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resource.h"

namespace BrokenEngine {
	class ResourceTexture : public Resource {
	public:
		ResourceTexture(uint UID, std::string source_file);
		~ResourceTexture();

		bool LoadInMemory() override;
		void FreeMemory() override;

		void SetTextureID(uint ID);
		uint GetTexID();

	public:
		std::string Texture_path;
		uint Texture_width = 0;
		uint Texture_height = 0;
	private:
		uint buffer_id = 0;

	private:
		void OnOverwrite() override;
		void OnDelete() override;
	};
}
#endif //__RESOURCE_TEXTURE_H__
