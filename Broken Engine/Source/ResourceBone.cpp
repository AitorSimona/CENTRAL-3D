#include "ResourceBone.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ImporterBone.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ResourceBone::ResourceBone(uint UID, const char* source_file) : Resource(Resource::ResourceType::BONE, UID, source_file)
{
	extension = ".bone";
	resource_file = BONES_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->boneTexID;

}

ResourceBone::~ResourceBone()
{
}

bool ResourceBone::LoadInMemory()
{
	bool ret = true;

	if (App->fs->Exists(resource_file.c_str()))
	{
		// --- Load mesh data ---
		char* buffer = nullptr;
		App->fs->Load(resource_file.c_str(), &buffer);
		char* cursor = buffer;

		// --- Skip name and ranges ---
		uint ranges;
		memcpy(&ranges, cursor, sizeof(ranges));
		cursor += sizeof(ranges) + ranges;

		// meshID
		memcpy(&this->meshID, cursor, sizeof(uint));
		cursor += sizeof(uint);

		//  --- Load NumWeights ---
		memcpy(&NumWeights, cursor, sizeof(uint));
		cursor += sizeof(uint);

		//  --- Load matrix ---
		float* matrix = new float[16];
		memcpy(matrix, cursor, sizeof(float) * 16);
		cursor += sizeof(float) * 16;

		this->matrix = float4x4(matrix[0], matrix[1], matrix[2], matrix[3], 
								matrix[4], matrix[5], matrix[6], matrix[7],
								matrix[8], matrix[9], matrix[10], matrix[11],
								matrix[12], matrix[13], matrix[14], matrix[15]);

		//  --- Load weights ---
		weight = new float[NumWeights];
		memcpy(weight, cursor, sizeof(float) * NumWeights);
		cursor += sizeof(float) * NumWeights;

		//  --- Load index_weights ---
		index_weight = new uint[NumWeights];
		memcpy(index_weight, cursor, sizeof(uint) * NumWeights);
		cursor += sizeof(uint) * NumWeights;


		delete[] matrix;
		delete[] buffer;
		
		cursor = nullptr;

		return ret;
	}
}

void ResourceBone::FreeMemory()
{
	if (weight)
	{
		delete[] weight;
		weight = nullptr;
	}
	
	if (index_weight)
	{
		delete[] index_weight;
		index_weight = nullptr;
	}
}

void ResourceBone::OnOverwrite()
{
	// Since mesh is not a standalone resource (which means it is always owned by a model) the model is in charge
	// of overwriting it (see ResourceModel OnOverwrite for details)
	NotifyUsers(ResourceNotificationType::Overwrite);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());
}

void ResourceBone::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceBone::Repath()
{
	ImporterBone* IBone = App->resources->GetImporter<ImporterBone>();
	IBone->Save(this);
}