#include "ResourceModel.h"

ResourceModel::ResourceModel(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::MODEL, UID, source_file, destination_file)
{
}

ResourceModel::~ResourceModel()
{
}
