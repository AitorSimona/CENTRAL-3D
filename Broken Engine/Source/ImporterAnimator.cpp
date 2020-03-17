#include "ImporterAnimator.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ComponentAnimation.h"
#include "ResourceAnimator.h"
#include "ResourceMeta.h"
#include "ImporterMeta.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ImporterAnimator::ImporterAnimator() : Importer(Importer::ImporterType::Animator)
{
}

ImporterAnimator::~ImporterAnimator()
{
}

Resource* ImporterAnimator::Import(ImportData& IData) const
{
	if (!App->fs->Exists(IData.path))
		return nullptr;

	ResourceAnimator* animator = (ResourceAnimator*)App->resources->CreateResource(Resource::ResourceType::ANIMATOR, IData.path);;

	// --- Create Meta ---
	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, animator->GetOriginalFile(), animator->GetUID());

	if (meta)
		IMeta->Save(meta);

	//Save(animator);

	return animator;
}


void ImporterAnimator::Save(ResourceAnimator* animator) const
{
	json node;

	for (int i = 0; i < animator->animations.size(); ++i)
	{
		node[animator->animations[i]->name]["name"] = animator->animations[i]->name;
		node[animator->animations[i]->name]["start_frame"] = std::to_string(animator->animations[i]->start);
		node[animator->animations[i]->name]["end_frame"] = std::to_string(animator->animations[i]->end);
		node[animator->animations[i]->name]["loop"] = animator->animations[i]->loop;
		node[animator->animations[i]->name]["default"] = animator->animations[i]->Default;
		//node[animator->animations[i]->name]["speed"] = animator->animations[i]->speed;
	}

	// --- Serialize JSON to string ---
	std::string data;
	App->GetJLoader()->Serialize(node, data);

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(animator->GetResourceFile(), buffer, size);
}


Resource* ImporterAnimator::Load(const char* path) const
{
	ResourceAnimator* animator = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	animator = App->resources->anim_info.find(meta->GetUID()) != App->resources->anim_info.end() ? App->resources->anim_info.find(meta->GetUID())->second : (ResourceAnimator*)App->resources->CreateResourceGivenUID(Resource::ResourceType::ANIMATOR, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(animator->GetOriginalFile())) 
	{
		animator->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(animator);
	}

	return animator;
}
