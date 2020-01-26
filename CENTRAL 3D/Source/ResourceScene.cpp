#include "ResourceScene.h"


ResourceScene::ResourceScene(uint UID, std::string source_file) : Resource(Resource::ResourceType::SCENE, UID, source_file)
{
	extension = ".scene";
	resource_file = SCENES_FOLDER + std::to_string(UID) + extension;
}

ResourceScene::~ResourceScene()
{
}
