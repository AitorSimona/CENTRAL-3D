#ifndef __MODULE_TEXTURES_H__
#define __MODULE_TEXTURES_H__

#include "Module.h"
#include "Globals.h"

#define CHECKERS_HEIGHT 32
#define CHECKERS_WIDTH 32

class ModuleTextures : public Module
{
public:

	ModuleTextures(bool start_enabled = true);
	~ModuleTextures();

	bool Init(json file);
	bool Start();
	bool CleanUp();

	uint LoadCheckImage() const;
	uint CreateTextureFromFile(const char* path, uint &width, uint &height, uint LibUID) const;
	uint CreateTextureFromPixels(int internalFormat, uint width, uint height, uint format, const void* pixels, bool CheckersTexture = false) const;

	uint GetCheckerTextureID() const;
private:

	uint CheckerTexID = 0;

private:
	// --- Called by CreateTextureFromPixels to split code ---
	inline void SetTextureParameters(bool CheckersTexture = false) const;

	inline void CreateTextureFromImage(uint & TextureID, uint &width, uint &height, const char* path, bool load_existing = false) const;
};

#endif