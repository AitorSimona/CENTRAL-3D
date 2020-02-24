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

	//Basic animation info
	resource_anim->duration = data.animation->mDuration;
	resource_anim->ticksPerSecond = data.animation->mTicksPerSecond;
	resource_anim->numChannels = data.animation->mNumChannels;

	//Creating space for channels
	resource_anim->channels = new Channel[data.animation->mNumChannels];

	//Loading channels info
	for (uint i = 0; i < data.animation->mNumChannels; i++)
	{
		for (uint j = 0; j < data.animation->mChannels[i]->mNumPositionKeys; j++)
			resource_anim->channels[i].PositionKeys[data.animation->mChannels[i]->mPositionKeys[j].mTime] = float3(data.animation->mChannels[i]->mPositionKeys[j].mValue.x,
				data.animation->mChannels[i]->mPositionKeys[j].mValue.y, data.animation->mChannels[i]->mPositionKeys[j].mValue.z);

		for (uint j = 0; j < data.animation->mChannels[i]->mNumRotationKeys; j++)
			resource_anim->channels[i].RotationKeys[data.animation->mChannels[i]->mRotationKeys[i].mTime] = Quat(data.animation->mChannels[i]->mRotationKeys[i].mValue.x,
				data.animation->mChannels[i]->mRotationKeys[i].mValue.y, data.animation->mChannels[i]->mRotationKeys[i].mValue.z, data.animation->mChannels[i]->mRotationKeys[i].mValue.w);

		for (uint j = 0; j < data.animation->mChannels[i]->mNumScalingKeys; j++)
			resource_anim->channels[i].ScaleKeys[data.animation->mChannels[i]->mScalingKeys[i].mTime] = float3(data.animation->mChannels[i]->mScalingKeys[i].mValue.x,
				data.animation->mChannels[i]->mScalingKeys[i].mValue.y, data.animation->mChannels[i]->mScalingKeys[i].mValue.z);

		resource_anim->channels[i].name = data.animation->mChannels[i]->mNodeName.C_Str();

		//------------------------------- Delete _$AssimpFbx$_ -----------------------
		// Search for the substring in string
		size_t pos = resource_anim->channels[i].name.find("_$AssimpFbx$_");

		if (pos != std::string::npos)
		{
			std::string ToDelete = "_$AssimpFbx$_";

			// If found then erase it from string
			resource_anim->channels[i].name.erase(pos, ToDelete.length());
		}
	}

	// --- Save to library ---
	Save(resource_anim);

	App->resources->AddResourceToFolder(resource_anim);


	return resource_anim;
}

void ImporterAnimation::Save(ResourceAnimation* anim) const
{
	//----------------------------- CALCULATE SIZE ----------------------------------

	//Animation Duration, TicksperSec, numChannels
	uint size = sizeof(float) + sizeof(float) + sizeof(uint);

	for (int i = 0; i < anim->numChannels; i++)
	{

		//Size name + Name
		size += sizeof(uint) + anim->channels[i].name.size() + sizeof(uint) * 3;

		//PositionsKeys size
		size += sizeof(double) * anim->channels[i].PositionKeys.size();
		size += sizeof(float) * 3 * anim->channels[i].PositionKeys.size();
		//RotationsKeys size
		size += sizeof(double) * anim->channels[i].RotationKeys.size();
		size += sizeof(float) * 4 * anim->channels[i].RotationKeys.size();
		//ScalesKeys size
		size += sizeof(double) * anim->channels[i].ScaleKeys.size();
		size += sizeof(float) * 3 * anim->channels[i].ScaleKeys.size();
	}
	//-------------------------------------------------------------------------------

	//---------------------------------- Allocate -----------------------------------
	char* data = new char[size];
	char* cursor = data;

	//Duration
	memcpy(cursor, &anim->duration, sizeof(float));
	cursor += sizeof(float);

	//TicksperSec
	memcpy(cursor, &anim->ticksPerSecond, sizeof(float));
	cursor += sizeof(float);

	//numChannels
	memcpy(cursor, &anim->numChannels, sizeof(uint));
	cursor += sizeof(uint);

	//Channels
	for (int i = 0; i < anim->numChannels; i++)
	{
		//Name size 
		uint SizeofName = anim->channels[i].name.size();
		memcpy(cursor, &SizeofName, sizeof(uint));
		cursor += sizeof(uint);

		//name data
		memcpy(cursor, anim->channels[i].name.c_str(), anim->channels[i].name.size());
		cursor += anim->channels[i].name.size();

		//Poskeys, Rotkeys and Scalekeys SIZES
		uint ranges[3] = { anim->channels[i].PositionKeys.size(), anim->channels[i].RotationKeys.size(), anim->channels[i].ScaleKeys.size() };
		memcpy(cursor, ranges, sizeof(ranges));
		cursor += sizeof(ranges);

		//PositionKeys
		std::map<double, float3>::const_iterator it = anim->channels[i].PositionKeys.begin();
		for (it = anim->channels[i].PositionKeys.begin(); it != anim->channels[i].PositionKeys.end(); it++)
		{
			memcpy(cursor, &it->first, sizeof(double));
			cursor += sizeof(double);

			memcpy(cursor, &it->second, sizeof(float) * 3);
			cursor += sizeof(float) * 3;
		}

		//RotationKeys
		std::map<double, Quat>::const_iterator it2 = anim->channels[i].RotationKeys.begin();
		for (it2 = anim->channels[i].RotationKeys.begin(); it2 != anim->channels[i].RotationKeys.end(); it2++)
		{
			memcpy(cursor, &it2->first, sizeof(double));
			cursor += sizeof(double);

			memcpy(cursor, &it2->second, sizeof(float) * 4);
			cursor += sizeof(float) * 4;
		}

		//ScaleKeys
		std::map<double, float3>::const_iterator it3 = anim->channels[i].ScaleKeys.begin();
		for (it3 = anim->channels[i].ScaleKeys.begin(); it3 != anim->channels[i].ScaleKeys.end(); it3++)
		{
			memcpy(cursor, &it3->first, sizeof(double));
			cursor += sizeof(double);

			memcpy(cursor, &it3->second, sizeof(float) * 3);
			cursor += sizeof(float) * 3;
		}
	}

	App->fs->Save(anim->GetResourceFile(), data, size);


	if (data)
	{
		delete[] data;
		data = nullptr;
		cursor = nullptr;
	}
}

Resource* ImporterAnimation::Load(const char* path) const
{

	Resource* anim = nullptr;
	

	return anim;
}
