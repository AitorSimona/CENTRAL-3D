#include "ResourceAnimator.h"

#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ModuleEventManager.h"

#include "ResourceTexture.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceAnimator::ResourceAnimator(uint UID, const char* source_file) : Resource(Resource::ResourceType::MATERIAL, UID, source_file)
{
	extension = ".animator";
	resource_file = ANIMATOR_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;
}

ResourceAnimator::~ResourceAnimator() {

}

bool ResourceAnimator::LoadInMemory() 
{
	

	return true;
}

void ResourceAnimator::FreeMemory() 
{
	for (uint i = 0; i < uniforms.size(); ++i) {
		delete uniforms[i];
	}

	uniforms.clear();
}

void ResourceAnimator::OnOverwrite()
{
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