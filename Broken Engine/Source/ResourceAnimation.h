#ifndef __RESOURCE_ANIMATION__
#define __RESOURCE_ANIMATION__

#include "Resource.h"
#include <map>
#include <string>
#include "Math.h"

BE_BEGIN_NAMESPACE
struct BROKEN_API Channel
{
	std::string name;

	//Position
	bool PosHasKey() const;
	std::map<double, float3> PositionKeys;
	std::map<double, float3>::iterator PrevPosition(double current);
	std::map<double, float3>::iterator NextPosition(double current);

	//Rotation
	bool RotHasKey() const;
	std::map<double, Quat> RotationKeys;
	std::map<double, Quat>::iterator PrevRotation(double current);
	std::map<double, Quat>::iterator NextRotation(double current);

	//Scale
	bool ScaleHasKey() const;
	std::map<double, float3> ScaleKeys;
	std::map<double, float3>::iterator PrevScale(double current);
	std::map<double, float3>::iterator NextScale(double current);
};

class BROKEN_API ResourceAnimation : public Resource
{
public:

	ResourceAnimation(uint UID, const char* source_file);
	~ResourceAnimation();

	bool LoadInMemory() override;
	void FreeMemory() override;

public:

	std::string name;

	float duration = 0.0f;
	float ticksPerSecond = 0.0f;

	uint numChannels = 0;
	Channel* channels = nullptr;

private:
	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};

BE_END_NAMESPACE
#endif // __RESOURCE_ANIMATION__