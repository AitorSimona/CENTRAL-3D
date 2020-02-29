#ifndef __PARTICLE_H__
#define __PARTICLE_H__

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "Globals.h" 

#include "Math.h"

using namespace physx;

class Particle {
public:
	Particle();
	~Particle();

	void Draw();

public:

	float3 position = { 0,0,0 };

	uint lifeTime=1;
	uint spawnTime = 1;
	float diameter=1;
};

#endif // __PARTICLE_H__