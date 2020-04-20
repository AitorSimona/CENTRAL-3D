#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Math.h"
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE

class ResourceTexture;

class BROKEN_API Particle {
public:
	Particle();
	~Particle();

	void Draw();

public:


	float3 position = { 0,0,0 };
	float4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
	uint lifeTime=1;
	uint spawnTime = 1;
	float diameter=1;
	float2 scale = {1,1};

	ResourceTexture* texture = nullptr;
};

BE_END_NAMESPACE
#endif // __PARTICLE_H__