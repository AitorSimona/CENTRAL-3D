#include "ResourceScene.h"


ResourceScene::ResourceScene(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::SCENE, UID, source_file, destination_file)
{
}

ResourceScene::~ResourceScene()
{
}
