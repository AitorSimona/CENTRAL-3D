#ifndef MODULE_PARTICLES_H__
#define MODULE_PARTICLES_H__

#include "Module.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

class ComponentParticleEmitter;

using namespace physx;

class ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled);
	~ModuleParticles();

	bool Init(json config) override;
	bool Start() override;
	update_status Update(float dt) override;

	void DrawParticles();

	void AddEmitter(ComponentParticleEmitter* componentEmitter);
	void DeleteEmitter(ComponentParticleEmitter* componentEmitter);

	bool CleanUp() override;

private:
	
	uint maxParticles=10000;
	bool perParticleRestOffset = false;

	PxParticleSystem* particleSystem = nullptr;

	std::vector<ComponentParticleEmitter*> particleEmitters;
};

#endif // _MODULE_PARTICLES_H__
