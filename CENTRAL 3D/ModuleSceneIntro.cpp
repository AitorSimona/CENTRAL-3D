#include "Globals.h"
#include "Application.h"
#include "ModuleSceneIntro.h"
#include "Primitive.h"

#include "Math.h"

#include "Source/mmgr/mmgr.h"

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

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	//--- Testing Sphere Intersection with MathGeoLib ---

	math::Sphere sphere;
	float3 spherepos = float3(0.0, 0.0, 0.0);
	sphere.pos = spherepos;
	sphere.r = 3.0f;

	math::OBB sph1obb;
	sph1obb.pos = spherepos;
	sph1obb.r = float3(sphere.r, sphere.r, sphere.r);

	math::AABB sph1AABB;
	sph1AABB.Enclose(sphere);


	math::Sphere sphere2;
	float3 sphere2pos = float3(50.0, 0.0, 0.0);
	sphere2.pos = sphere2pos;
	sphere2.r = 3.0f;

	math::OBB sph2obb;
	sph2obb.pos = sphere2pos;
	sph2obb.r = float3(sphere2.r, sphere2.r, sphere2.r);
	bool obbintersection = sph2obb.Contains(sph1obb);

	math::AABB sph2AABB;
	sph2AABB.Enclose(sphere2);

	//bool intersection = sphere.Intersects(sphere2);

	//bool obbintersection = sph1obb.Intersects(sph2obb);

	bool aabbintersection = sph1AABB.Intersects(sph2AABB);

	// Grid
	PlaneP p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	
	return UPDATE_CONTINUE;
}



