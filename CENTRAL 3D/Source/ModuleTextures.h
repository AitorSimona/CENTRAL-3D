#ifndef __MODULE_TEXTURES_H__
#define __MODULE_TEXTURES_H__

#include "Module.h"
#include "Globals.h"

class ModuleTextures : public Module
{
public:

	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool Init(json file);
	bool Start();
	bool CleanUp();

	uint LoadCheckImage() const;
	uint CreateTextureFromFile(const char* path) const;
	uint CreateTextureFromPixels(int internalFormat, uint width, uint height, uint format, const void* pixels, bool CheckersTexture = false) const;

	uint GetCheckerTextureID() const;
private:

	uint CheckerTexID = 0;

private:

	inline void SetTextureParameters(bool CheckersTexture = false) const;
};

#endif