#ifndef __SCRIPTINGPARTICLES_H__
#define __SCRIPTINGPARTICLES_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingParticles {
public:
	ScriptingParticles();
	~ScriptingParticles();

public:
	void ActivateParticleEmitter(uint gameobject_UUID) const;
	void DeactivateParticleEmitter(uint gameobject_UUID) const;

	void PlayParticleEmitter(uint gameobject_UUID);
	void StopParticleEmitter(uint gameobject_UUID);
	void SetEmissionRateFromScript(float ms, uint gameobject_UUID);
	void SetParticlesPerCreationFromScript(int particlesAmount, uint gameobject_UUID);

	void SetParticleAcceleration(float x, float y, float z, uint gameobject_UUID);
	void SetParticleVelocityFromScript(float x, float y, float z, uint gameobject_UUID);
	void SetRandomParticleVelocity(float x, float y, float z, uint gameobject_UUID);

	void SetParticleLooping(bool active, uint gameobject_UUID);
	void SetParticleDuration(int duration, uint gameobject_UUID);
	void SetParticleLifeTime(int ms, uint gameobject_UUID);

	void SetParticleScaleFromScript(float x, float y, uint gameobject_UUID);
	void SetRandomParticleScale(float randomFactor, uint gameobject_UUID);


};
BE_END_NAMESPACE
#endif // __SCRIPTINGPARTICLES_H__