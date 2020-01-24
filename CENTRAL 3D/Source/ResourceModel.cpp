#include "ResourceModel.h"

ResourceModel::ResourceModel() : Resource(Resource::ResourceType::MODEL)
{
}

ResourceModel::ResourceModel(uint UID) : Resource(Resource::ResourceType::MODEL, UID)
{
}

ResourceModel::~ResourceModel()
{
}
