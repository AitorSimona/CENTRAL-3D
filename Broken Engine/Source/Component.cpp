#include "Component.h"
#include "GameObject.h"
#include "Application.h"

#include "mmgr/mmgr.h"

using namespace Broken;
Component::Component(GameObject* ContainerGO, Component::ComponentType type) {
	GO = ContainerGO;
	this->type = type;
	UID = App->GetRandom().Int();
	Enable();
}

Component::~Component() {
	Disable();
}

void Component::Enable() {
	active = true;
}

void Component::Disable() {
	active = false;
}

bool Component::IsEnabled() const {
	return active;
}

bool& Component::GetActive() {
	return active;
}

Component::ComponentType Component::GetType() const {
	return type;
}

GameObject* Component::GetContainerGameObject() const {
	return GO;
}
