#include "ResourceScene.h"

ResourceScene::ResourceScene() : Resource(Resource::ResourceType::SCENE)
{
}

ResourceScene::ResourceScene(uint UID) : Resource(Resource::ResourceType::SCENE, UID)
{
}

ResourceScene::~ResourceScene()
{
}
