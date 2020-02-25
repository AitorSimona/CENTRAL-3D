#include "ImporterBone.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ResourceBone.h"

#include "Assimp/include/scene.h"

#include "Math.h"

#include "mmgr/mmgr.h"


ImporterBone::ImporterBone() : Importer(Importer::ImporterType::Bone)
{
}

ImporterBone::~ImporterBone()
{
}

Resource* ImporterBone::Import(ImportData& IData) const
{
	ImportBoneData data = (ImportBoneData&)IData;

	ResourceBone* resource_bone = (ResourceBone*)App->resources->CreateResource(Resource::ResourceType::BONE, IData.path);

	//TODO: get the correct id
	//resource_bone->meshID = 
	resource_bone->NumWeights = data.bone->mNumWeights;
	resource_bone->matrix = float4x4(data.bone->mOffsetMatrix.a1, data.bone->mOffsetMatrix.a2, data.bone->mOffsetMatrix.a3, data.bone->mOffsetMatrix.a4,
									 data.bone->mOffsetMatrix.b1, data.bone->mOffsetMatrix.b2, data.bone->mOffsetMatrix.b3, data.bone->mOffsetMatrix.b4,
									 data.bone->mOffsetMatrix.c1, data.bone->mOffsetMatrix.c2, data.bone->mOffsetMatrix.c3, data.bone->mOffsetMatrix.c4,
									 data.bone->mOffsetMatrix.d1, data.bone->mOffsetMatrix.d2, data.bone->mOffsetMatrix.d3, data.bone->mOffsetMatrix.d4);

	resource_bone->weight = new float[resource_bone->NumWeights];
	resource_bone->index_weight = new uint[resource_bone->NumWeights];

	for (int i = 0; i < resource_bone->NumWeights; i++)
	{
		resource_bone->weight[i] = data.bone->mWeights[i].mWeight;
		resource_bone->index_weight[i] = data.bone->mWeights[i].mVertexId;
	}

	uint size = sizeof(uint) + sizeof(uint) + sizeof(float) * 16 + resource_bone->NumWeights * sizeof(float) + resource_bone->NumWeights * sizeof(uint);

	char* my_data = new char[size];
	char* cursor = my_data;

	// meshID
	memcpy(cursor, &resource_bone->meshID, sizeof(uint));
	cursor += sizeof(uint);

	// numWeights
	memcpy(cursor, &resource_bone->NumWeights, sizeof(uint));
	cursor += sizeof(uint);

	// matrix
	memcpy(cursor, &resource_bone->matrix, sizeof(float) * 16);
	cursor += sizeof(float) * 16;

	//Weights
	memcpy(cursor, resource_bone->weight, sizeof(float) * resource_bone->NumWeights);
	cursor += sizeof(float) * resource_bone->NumWeights;

	//index_weights
	memcpy(cursor, resource_bone->index_weight, sizeof(uint) * resource_bone->NumWeights);
	cursor += sizeof(uint) * resource_bone->NumWeights;

	return resource_bone;
}

void ImporterBone::Save(ResourceBone* anim) const
{

}

Resource* ImporterBone::Load(const char* path) const
{
	return nullptr;
}