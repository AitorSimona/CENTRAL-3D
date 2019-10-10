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


	cube = new PrimitiveCube(1.0f, 1.0f, 1.0f);
	cube->axis = true;
	cube->SetPos(0.0f, 0.0f, 0.0f);
	cube->SetRotation(30,float3::unitX);

	sphere = new PrimitiveSphere();
	cube->axis = true;
	cube->SetPos(0.0f, 0.0f, 0.0f);


	grid = new PrimitiveGrid();
	grid->axis = true;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	delete cube;
	delete sphere;
	delete grid;

	return true;
}

bool ModuleSceneIntro::Draw()
{

	//cube->Render();
	sphere->Render();
	grid->Render();

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{

	
	return UPDATE_CONTINUE;
}



