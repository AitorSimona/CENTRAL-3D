#include "ComponentCamera.h"

ComponentCamera::ComponentCamera(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Camera)
{
}

ComponentCamera::~ComponentCamera()
{
}
