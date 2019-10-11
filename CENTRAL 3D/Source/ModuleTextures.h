#ifndef __MODULE_TEXTURES_H__
#define __MODULE_TEXTURES_H__

#include "Module.h"
#include "Globals.h"
#include "JSONLoader.h"

class ModuleTextures : public Module
{
public:

	ModuleTextures(Application* app, bool start_enabled = true);
	~ModuleTextures();

	bool Init(json file);
	bool Start();
	bool CleanUp();

	// Images

private:

};

#endif