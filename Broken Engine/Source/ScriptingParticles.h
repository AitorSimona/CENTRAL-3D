#ifndef __SCRIPTINGPARTICLES_H__
#define __SCRIPTINGPARTICLES_H__

#include "BrokenCore.h"

class lua_State;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingParticles {
public:
	ScriptingParticles();
	~ScriptingParticles();

public:
	void ActivateParticleEmitter(uint gameobject_UUID = -1) const;
	void DeactivateParticleEmitter(uint gameobject_UUID = -1) const;

	void PlayParticleEmitter(uint gameobject_UUID = -1);
	void StopParticleEmitter(uint gameobject_UUID = -1);
	void SetEmissionRateFromScript(float ms, uint gameobject_UUID = -1);
	void SetParticlesPerCreationFromScript(int particlesAmount, uint gameobject_UUID = -1);

	void SetParticleAcceleration(float x, float y, float z, uint gameobject_UUID = -1);
	void SetParticleVelocityFromScript(float x, float y, float z, uint gameobject_UUID = -1);
	void SetRandomParticleVelocity(float x, float y, float z, uint gameobject_UUID = -1);

	void SetParticleLooping(bool active, uint gameobject_UUID = -1);
	void SetParticleDuration(int duration, uint gameobject_UUID = -1);
	void SetParticleLifeTime(int ms, uint gameobject_UUID = -1);

	void SetParticleScaleFromScript(float x, float y, uint gameobject_UUID = -1);
	void SetRandomParticleScale(float randomFactor, uint gameobject_UUID = -1);


};
BE_END_NAMESPACE
#endif // __SCRIPTINGPARTICLES_H__