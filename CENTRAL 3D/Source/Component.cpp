#include "Component.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

Component::Component(GameObject* ContainerGO,Component::ComponentType type) 
{
	GO = ContainerGO;
	this->type = type;
}

Component::~Component()
{
}
