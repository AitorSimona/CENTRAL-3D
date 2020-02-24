#include "ResourceAnimation.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "mmgr/mmgr.h"


ResourceAnimation::ResourceAnimation(uint UID, std::string source_file) : Resource(Resource::ResourceType::ANIMATION, UID, source_file)
{
	extension = ".anim";
	resource_file = ANIMATIONS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->defaultfileTexID;

}

ResourceAnimation::~ResourceAnimation()
{
}

bool ResourceAnimation::LoadInMemory()
{
	return true;
}

void ResourceAnimation::FreeMemory()
{

}

void ResourceAnimation::OnOverwrite()
{
	// Since mesh is not a standalone resource (which means it is always owned by a model) the model is in charge
	// of overwriting it (see ResourceModel OnOverwrite for details)
	NotifyUsers(ResourceNotificationType::Overwrite);
	FreeMemory();
	App->fs->Remove(resource_file.c_str());


}

void ResourceAnimation::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceAnimation::Repath()
{
	/*ImporterMesh* IMesh = App->resources->GetImporter<ImporterMesh>();
	IMesh->Save(this);*/
}


//------------------------ CHANNEL FUNCTIONS -----------------------------------//


bool Channel::PosHasKey() const
{
	return !PositionKeys.empty();
}

std::map<double, float3>::iterator Channel::PrevPosition(double current)
{
	std::map<double, float3>::iterator ret = PositionKeys.lower_bound(current);
	if (ret != PositionKeys.begin())
		ret--;

	return ret;
}

std::map<double, float3>::iterator Channel::NextPosition(double current)
{
	return PositionKeys.upper_bound(current);
}

bool Channel::RotHasKey() const
{
	return !RotationKeys.empty();
}

std::map<double, Quat>::iterator Channel::PrevRotation(double current)
{
	std::map<double, Quat>::iterator ret = RotationKeys.lower_bound(current);
	if (ret != RotationKeys.begin())
		ret--;
	return ret;
}

std::map<double, Quat>::iterator Channel::NextRotation(double current)
{
	return RotationKeys.upper_bound(current);
}

bool Channel::ScaleHasKey() const
{
	return !ScaleKeys.empty();
}

std::map<double, float3>::iterator Channel::PrevScale(double current)
{
	std::map<double, float3>::iterator ret = ScaleKeys.lower_bound(current);
	if (ret != ScaleKeys.begin())
		ret--;
	return ret;
}

std::map<double, float3>::iterator Channel::NextScale(double current)
{
	return ScaleKeys.upper_bound(current);
}