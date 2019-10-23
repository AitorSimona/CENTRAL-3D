#include "Component.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

Component::Component(GameObject* ContainerGO,Component::ComponentType type) 
{
	GO = ContainerGO;
	this->type = type;
	Enable();
}

Component::Component(Component::ComponentType type)
{
	this->type = type;
	Enable();
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

bool Component::IsEnabled()
{
	return active;
}

bool & Component::GetActive()
{
	return active;
}

Component::ComponentType Component::GetType()
{
	return type;
}

GameObject * Component::GetContainerGameObject()
{
	return GO;
}
