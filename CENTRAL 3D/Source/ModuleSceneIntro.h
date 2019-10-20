#pragma once
#include "Module.h"
#include "Globals.h"

//class PrimitiveCube;
//class PrimitiveSphere;
//class PrimitiveGrid;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	bool Draw();

	//PrimitiveCube* cube = nullptr;

	//PrimitiveSphere* sphere = nullptr;

	//PrimitiveGrid* grid = nullptr;
};
