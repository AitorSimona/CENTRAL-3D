#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(GameObject * ContainerGO) : Component(ContainerGO, Component::ComponentType::Material)
{
}

ComponentMaterial::~ComponentMaterial()
{
}
