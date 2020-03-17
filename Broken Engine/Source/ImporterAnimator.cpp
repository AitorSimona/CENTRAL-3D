#include "ImporterAnimator.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

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
	//json node;

	//for (int i = 0; i < animations.size(); ++i)
	//{
	//	node[animations[i]->name]["name"] = animations[i]->name;
	//	node[animations[i]->name]["start_frame"] = std::to_string(animations[i]->start);
	//	node[animations[i]->name]["end_frame"] = std::to_string(animations[i]->end);
	//	node[animations[i]->name]["loop"] = std::to_string(animations[i]->loop);
	//	node[animations[i]->name]["speed"] = std::to_string(animations[i]->speed);
	//}

	//// --- Serialize JSON to string ---
	//std::string data;
	//App->GetJLoader()->Serialize(node, data);

	//// --- Finally Save to file ---
	//char* buffer = (char*)data.data();
	//uint size = data.length();

	//App->fs->Save(animation_path.c_str(), buffer, size);
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
