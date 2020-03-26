#include "ResourceAnimation.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ImporterAnimation.h"

#include "mmgr/mmgr.h"
#include "Math.h"


using namespace Broken;

ResourceAnimation::ResourceAnimation(uint UID, const char* source_file) : Resource(Resource::ResourceType::ANIMATION, UID, source_file)
{
	extension = ".anim";
	resource_file = ANIMATIONS_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->animationTexID;

}

ResourceAnimation::~ResourceAnimation()
{
}

bool ResourceAnimation::LoadInMemory()
{
	bool ret = true;

	if (App->fs->Exists(resource_file.c_str()))
	{
		char* buffer = nullptr;
		uint bytes = 0;

		App->fs->Load(resource_file.c_str(), &buffer);
		char* cursor = buffer;

		//skip name length and name
		uint length = 0;
		memcpy(&length, cursor, sizeof(length));
		cursor += sizeof(length) + length;

		uint anim_name_size = 0;
		memcpy(&anim_name_size, cursor, sizeof(uint));
		cursor += sizeof(uint);
		
		//Get animation name
		char* string = new char[anim_name_size + 1];
		bytes = sizeof(const char) * anim_name_size;
		memcpy(string, cursor, bytes);
		cursor += bytes;

		string[anim_name_size] = '\0';
		this->name = string;
		delete[] string;

		//Duration
		memcpy(&this->duration, cursor, sizeof(float));
		cursor += sizeof(float);

		//TicksperSec
		memcpy(&this->ticksPerSecond, cursor, sizeof(float));
		cursor += sizeof(float);

		//numChannels
		memcpy(&this->numChannels, cursor, sizeof(uint));
		cursor += sizeof(uint);


		this->channels = new Channel[this->numChannels];
		for (uint i = 0; i < this->numChannels; i++)
		{
			bytes = 0;

			// Load channel name -----------------------
			uint SizeofName = 0;
			memcpy(&SizeofName, cursor, sizeof(uint));
			cursor += sizeof(uint);

			char* string = new char[SizeofName + 1];
			bytes = sizeof(char) * SizeofName;
			memcpy(string, cursor, bytes);
			cursor += bytes;

			string[SizeofName] = '\0';
			this->channels[i].name = string;
			delete[] string;
			//-------------------------------------------

			//Poskeys, Rotkeys and Scalekeys SIZES
			uint ranges[3];
			memcpy(&ranges, cursor, sizeof(uint) * 3);
			cursor += sizeof(uint) * 3;

			//Load PosKeys
			for (int j = 0; j < ranges[0]; j++)
			{
				double key;
				memcpy(&key, cursor, sizeof(double));
				cursor += sizeof(double);

				float pos[3];
				memcpy(&pos, cursor, sizeof(float) * 3);
				cursor += sizeof(float) * 3;

				this->channels[i].PositionKeys[key] = (float3)pos;
			}

			//Load RotKeys
			for (int j = 0; j < ranges[1]; j++)
			{
				double key;
				memcpy(&key, cursor, sizeof(double));
				cursor += sizeof(double);

				float rot[4];
				memcpy(&rot, cursor, sizeof(float) * 4);
				cursor += sizeof(float) * 4;

				this->channels[i].RotationKeys[key] = (Quat)rot;
			}

			//Load ScaleKeys
			for (int j = 0; j < ranges[2]; j++)
			{
				double key;
				memcpy(&key, cursor, sizeof(double));
				cursor += sizeof(double);

				float scale[3];
				memcpy(&scale, cursor, sizeof(float) * 3);
				cursor += sizeof(float) * 3;

				this->channels[i].ScaleKeys[key] = (float3)scale;
			}

		}

		// --- Delete buffer data ---
		if (buffer)
		{
			delete[] buffer;
			buffer = nullptr;
			cursor = nullptr;
		}
	}
	return ret;
}

void ResourceAnimation::FreeMemory()
{
	if (channels)
	{
		delete[] this->channels;
		this->channels = nullptr;
	}
	
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
	ImporterAnimation* IAnim = App->resources->GetImporter<ImporterAnimation>();
	IAnim->Save(this);
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