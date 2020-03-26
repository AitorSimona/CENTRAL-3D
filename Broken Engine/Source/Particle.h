#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "Math.h"
#include "BrokenCore.h"

#include "ResourceTexture.h"

BE_BEGIN_NAMESPACE

class ResourceTexture;

class BROKEN_API Particle {
public:
	Particle();
	~Particle();

	void Draw();

public:


	float3 position = { 0,0,0 };
	float3 color = { 255.0f, 0.0f, 0.0f };
	uint lifeTime=1;
	uint spawnTime = 1;
	float diameter=1;

	ResourceTexture* texture = nullptr;
};

BE_END_NAMESPACE
#endif // __PARTICLE_H__