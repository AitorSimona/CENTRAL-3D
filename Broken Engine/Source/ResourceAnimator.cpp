#include "ResourceAnimator.h"

#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleEventManager.h"
#include "ComponentAnimation.h"

#include "ResourceTexture.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceAnimator::ResourceAnimator(uint UID, const char* source_file) : Resource(Resource::ResourceType::ANIMATOR, UID, source_file)
{
	extension = ".animator";
	std::string file = source_file;
	resource_file = file;

	previewTexID = App->gui->animatorTexID;
}

ResourceAnimator::~ResourceAnimator() 
{

}

bool ResourceAnimator::LoadInMemory() 
{
	std::string tmp = resource_file.c_str();
	json file = App->GetJLoader()->Load(tmp.c_str());

	if (!file.is_null())
	{
		for (json::iterator it = file.begin(); it != file.end(); ++it)
		{
			std::string name = file[it.key().c_str()]["name"];
			std::string start_frm = file[it.key().c_str()]["start_frame"];
			std::string end_frm = file[it.key().c_str()]["end_frame"];
			bool is_loop = file[it.key().c_str()]["loop"];
			bool Default = file[it.key().c_str()]["default"];
			//std::string speed = file[it.key().c_str()]["speed"];
			
			Animation* anim = new Animation(name, std::stoi(start_frm), std::stoi(end_frm), is_loop, Default);
			animations.push_back(anim);
		}
	}

	return true;
}

void ResourceAnimator::FreeMemory() 
{
	for (int i = 0; i < animations.size(); i++)
	{
		if (animations[i])
		{
			delete animations[i];
			animations[i] = nullptr;
		}

	}

	animations.clear();
}

void ResourceAnimator::OnOverwrite()
{
	FreeMemory();
	LoadInMemory();
	NotifyUsers(ResourceNotificationType::Overwrite);
}

void ResourceAnimator::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());


	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceAnimator::Repath()
{
	resource_file = original_file + extension;
}