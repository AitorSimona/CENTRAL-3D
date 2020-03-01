#ifndef __RESOURCE_BONE_H__
#define __RESOURCE_BONE_H__

#include "Resource.h"
#include "Globals.h"
#include "Math.h"

class ResourceBone : public Resource
{
public:

	ResourceBone(uint UID, std::string source_file);
	~ResourceBone();

	bool LoadInMemory() override;
	void FreeMemory() override;

public:

	uint meshID = 0;

	uint NumWeights = 0;
	float* weight = nullptr;
	uint* index_weight = nullptr;

	float4x4 matrix = float4x4::identity;

private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};

#endif //__RESOURCE_BONE_H__