#include "ModuleEventManager.h"

#include "mmgr/mmgr.h"

ModuleEventManager::ModuleEventManager(bool start_enabled)
{
}

ModuleEventManager::~ModuleEventManager()
{
}

bool ModuleEventManager::Init(json file)
{
	return true;
}

bool ModuleEventManager::Start()
{
	return true;
}

update_status ModuleEventManager::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventManager::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleEventManager::CleanUp()
{
	return true;
}
