#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleTextures.h"
#include "OpenGL.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceTexture::ResourceTexture(uint UID, const char* source_file) : Resource(Resource::ResourceType::TEXTURE, UID, source_file) {
	extension = ".dds";
	resource_file = TEXTURES_FOLDER + std::to_string(UID) + extension;
	buffer_id = App->textures->GetDefaultTextureID();
	previewTexID = App->gui->defaultfileTexID;

	// --- Force Texture memory load so we can display a preview ---
	LoadToMemory();
}

ResourceTexture::~ResourceTexture()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}

bool ResourceTexture::LoadInMemory()
{
	if (App->resources->IsFileImported(original_file.c_str()) && App->fs->Exists(resource_file.c_str()))
	{
		SetTextureID(App->textures->CreateTextureFromFile(resource_file.c_str(), Texture_width, Texture_height, -1));
	}
	else if (original_file != "DefaultTexture")
	{
		SetTextureID(App->textures->CreateTextureFromFile(original_file.c_str(), Texture_width, Texture_height, UID));
	}

	return true;
}

void ResourceTexture::FreeMemory()
{
	glDeleteTextures(1, (GLuint*)&buffer_id);
}

void ResourceTexture::OnOverwrite()
{
	NotifyUsers(ResourceNotificationType::Overwrite);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	SetTextureID(App->textures->CreateTextureFromFile(original_file.c_str(), Texture_width, Texture_height, UID));
}

void ResourceTexture::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}