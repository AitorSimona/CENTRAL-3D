#ifndef __RESOURCE_TEXTURE_H__
#define __RESOURCE_TEXTURE_H__

#include "Resource.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ResourceTexture : public Resource
{
public:

	ResourceTexture(uint UID, const char* source_file);
	~ResourceTexture();

	bool LoadInMemory() override;
	void FreeMemory() override;

	void SetTextureID(uint ID) { buffer_id = previewTexID = ID; }
	uint GetTexID() { return buffer_id; }

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
BE_END_NAMESPACE
#endif //__RESOURCE_TEXTURE_H__
