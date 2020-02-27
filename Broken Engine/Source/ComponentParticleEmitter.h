#ifndef __COMPONENT_PARTICLE_EMITTER__
#define __COMPONENT_PARTICLE_EMITTER__

#include "Component.h"
#include "Math.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

class Particle;

using namespace  physx;

class ComponentParticleEmitter : public Component
{
public:

	ComponentParticleEmitter(GameObject* ContainerGO);
	~ComponentParticleEmitter();

	void Enable() override;
	void Disable() override;

	void UpdateParticles(float dt);
	void DrawParticles();

	static inline Component::ComponentType GetType() { return Component::ComponentType::ParticleEmitter; };

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

private:
	
	PxParticleSystem* particleSystem=nullptr;

	std::vector<Particle*> particles;

	unsigned int maxParticles = 2000;
	bool perParticleRestOffset = false;

	PxParticleExt::IndexPool* indexPool;

	//Emitter properties
	PxVec3 size = { 0,0,0 };
	float emisionRate=500.0f;	//in milliseconds
	uint validParticles=0;
	PxVec3 externalAcceleration = {0,1000,0};
	PxVec3 particlesVelocity = { 10,10,10 };

	//Particle properties
	int particlesLifeTime=1000;

	float spawnClock=0.0f;
};

#endif //__COMPONENT_PARTICLE_EMITTER__