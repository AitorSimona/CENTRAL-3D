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

	bool Init(json file) override;
	bool Start() override;
	bool CleanUp() override;

	uint CreateTextureFromFile(const char* path, uint &width, uint &height, int UID = -1) const;
	uint CreateTextureFromPixels(int internalFormat, uint width, uint height, uint format, const void* pixels, bool CheckersTexture = false) const;

	uint GetCheckerTextureID() const;
	uint GetDefaultTextureID() const;

	uint CreateAndSaveTextureFromPixels(uint UID, int internalFormat, uint width, uint height, uint format, const void* pixels, std::string& out_path);

private:
	uint LoadCheckImage() const;
	uint LoadDefaultTexture() const;

	uint CheckerTexID = 0;
	uint DefaultTexture = 0;

private:
	// --- Called by CreateTextureFromPixels to split code ---
	inline void SetTextureParameters(bool CheckersTexture = false) const;

	inline void CreateTextureFromImage(uint & TextureID, uint &width, uint &height, std::string& path) const;
};

#endif