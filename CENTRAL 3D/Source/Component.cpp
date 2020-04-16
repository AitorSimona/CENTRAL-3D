#include "Component.h"
#include "GameObject.h"
#include "Application.h"

#include "mmgr/mmgr.h"

Component::Component(GameObject* ContainerGO,Component::ComponentType type) 
{
	GO = ContainerGO;
	this->type = type;
	Enable();
	UID = App->GetRandom().Int();
}

Component::~Component()
{
	Disable();
}

void Component::Enable()
{
	active = true;
}

void Component::Disable()
{
	active = false;
}

bool Component::IsEnabled() const
{
	return active;
}

void Component::SetUID(uint uid)
{
	UID = uid;
}

bool & Component::GetActive()
{
	return active;
}

uint Component::GetUID()
{
	return UID;
}

Component::ComponentType Component::GetType() const
{
	return type;
}

GameObject * Component::GetContainerGameObject() const
{
	return GO;
}
