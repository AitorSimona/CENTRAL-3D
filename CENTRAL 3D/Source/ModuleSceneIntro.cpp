#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "Primitive.h"
#include "ModuleRenderer3D.h"
#include "OpenGL.h"


#include "Math.h"

#include "mmgr/mmgr.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

bool ModuleSceneIntro::Draw()
{

	// Grid TODO: Remove and make own primitives class
	PlaneP p(0, 1, 0, 0);
	p.axis = true;
	p.Render();
	
	CylinderC c;
	c.radius = 3;
	c.SetPos(0, 0, 0);
	c.Render();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	//// Grid TODO: Remove and make own primitives class
	//PlaneP p(0, 1, 0, 0);
	//p.axis = true;
	//p.Render();

	
	return UPDATE_CONTINUE;
}



