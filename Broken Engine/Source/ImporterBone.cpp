#include "ImporterBone.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ResourceBone.h"

#include "Assimp/include/scene.h"

#include "Math.h"

#include "mmgr/mmgr.h"

ImporterBone::ImporterBone() : Importer(Importer::ImporterType::Bone)
{
}

ImporterBone::~ImporterBone()
{
}

Resource* ImporterBone::Import(ImportData& IData) const
{
	ImportBoneData data = (ImportBoneData&)IData;

	ResourceBone* resource_bone = (ResourceBone*)App->resources->CreateResource(Resource::ResourceType::BONE, IData.path);

	return resource_bone;
}

void ImporterBone::Save(ResourceBone* anim) const
{

}

Resource* ImporterBone::Load(const char* path) const
{
	return nullptr;
}