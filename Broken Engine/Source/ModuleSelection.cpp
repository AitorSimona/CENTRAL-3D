#include "ModuleSelection.h"


using namespace Broken;

ModuleSelection::ModuleSelection(bool start_enabled)
{
	name = "Module Selection";
}

ModuleSelection::~ModuleSelection()
{
}

bool ModuleSelection::Init(json& file)
{
	return true;
}

bool ModuleSelection::Start()
{
	return true;
}

update_status ModuleSelection::PreUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSelection::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSelection::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}
