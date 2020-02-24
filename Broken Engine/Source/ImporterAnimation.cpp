#include "ImporterAnimation.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ResourceAnimation.h"

#include "Assimp/include/scene.h"

#include "Math.h"

#include "mmgr/mmgr.h"

ImporterAnimation::ImporterAnimation() : Importer(Importer::ImporterType::Animation)
{
}

ImporterAnimation::~ImporterAnimation()
{
}

Resource* ImporterAnimation::Import(ImportData& IData) const
{
	ImportAnimationData data = (ImportAnimationData&)IData;

	ResourceAnimation* resource_anim = (ResourceAnimation*)App->resources->CreateResource(Resource::ResourceType::ANIMATION, IData.path);

	return resource_anim;
}

void ImporterAnimation::Save(ResourceAnimation* anim) const
{

}

Resource* ImporterAnimation::Load(const char* path) const
{
	return nullptr;
}
