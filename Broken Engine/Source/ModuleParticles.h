#ifndef MODULE_PARTICLES_H__
#define MODULE_PARTICLES_H__

#include "Module.h"


#include "PhysX_3.4/Include/PxPhysicsAPI.h"

using namespace physx;

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled);
	~ModuleParticles();

	bool Init(json config) override;
	bool Start() override;
	update_status Update(float dt) override;


	bool CleanUp() override;

private:
	
	uint maxParticles=100;
	bool perParticleRestOffset = false;

	PxParticleSystem* particleSystem = nullptr;
};

#endif // _MODULE_PARTICLES_H__
