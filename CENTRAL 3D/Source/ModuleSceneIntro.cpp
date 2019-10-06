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

	return true;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	for (int max_linesgrid = -30; max_linesgrid < 30; max_linesgrid++)
	{
		glVertex3f((float)max_linesgrid, 0.0f, -30.0f);
		glVertex3f((float)max_linesgrid, 0.0f, 30.0f);
		glVertex3f(-30.0f, 0.0f, (float)max_linesgrid);
		glVertex3f(30.0f, 0.0f, (float)max_linesgrid);

		glLineWidth(2.0f);
	}

	glEnd();


	PrimitiveCube cube;
	cube.axis = true;
	float3 size = float3(3.0f, 3.0f, 3.0f);
	cube.size = size;
	cube.SetPos(0.0f, 0.0f, 0.0f);
	cube.Render();
	
	return UPDATE_CONTINUE;
}



