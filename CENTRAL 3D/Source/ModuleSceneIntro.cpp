#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"


#include "Math.h"

#include "mmgr/mmgr.h"

ModuleSceneIntro::ModuleSceneIntro(bool start_enabled) : Module(start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	/*LOG("Loading Intro assets");*/
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	/*LOG("Unloading Intro scene");*/


	return true;
}

bool ModuleSceneIntro::Draw()
{

	return true;
}

update_status ModuleSceneIntro::Update(float dt)
{

	
	return UPDATE_CONTINUE;
}



