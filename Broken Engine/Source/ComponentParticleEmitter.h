#ifndef __COMPONENT_PARTICLE_EMITTER__
#define __COMPONENT_PARTICLE_EMITTER__

#include "Component.h"
#include "Math.h"

#include <random>

#include "PhysX_3.4/Include/PxPhysicsAPI.h"



class Particle;

using namespace  physx;

class ComponentParticleEmitter : public Component
{
	friend class ModuleParticles;
public:

	ComponentParticleEmitter(GameObject* ContainerGO);
	~ComponentParticleEmitter();

	void Enable() override;
	void Disable() override;

	void UpdateParticles(float dt);
	void DrawParticles();
	void ChangeParticlesColor(float3 color);

	static inline Component::ComponentType GetType() { return Component::ComponentType::ParticleEmitter; };

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void CreateInspectorNode() override;

private:

	double GetRandomValue(double min, double max); //MUST EREASE IN THE FUTURE

private:
	
	//Random number generation - EREASE IN THE FUTURE
	std::uniform_real_distribution<double> m_DoubleDistribution;
	std::random_device m_RandomDevice;
	std::default_random_engine m_RNEngine;
	PxParticleSystem* particleSystem=nullptr;

	std::vector<Particle*> particles;

	unsigned int maxParticles = 2000;
	bool perParticleRestOffset = false;

	PxParticleExt::IndexPool* indexPool;

	//Emitter properties
	PxVec3 size = { 0,0,0 };
	float emisionRate=500.0f;	//in milliseconds
	uint validParticles=0;
	PxVec3 externalAcceleration = {0,10,0};
	PxVec3 particlesVelocity = { 0,0,0 };
	PxVec3 velocityRandomFactor = {5,5,5};
	
	//Particle properties
	int particlesLifeTime=1000;
	float particlesSize = 1;

	float3 particlesColor = { 255, 255, 255 };

	float spawnClock=0.0f;
};

#endif //__COMPONENT_PARTICLE_EMITTER__