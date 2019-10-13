#include "ComponentRenderer.h"

ComponentRenderer::ComponentRenderer(GameObject* ContainerGO): Component(ContainerGO, Component::ComponentType::Renderer)
{
}

ComponentRenderer::~ComponentRenderer()
{
}
