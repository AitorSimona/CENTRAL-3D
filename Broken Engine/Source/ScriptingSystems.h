#ifndef __SCRIPTINGSYSTEMS_H__
#define __SCRIPTINGSYSTEMS_H__

#include "ModuleInput.h"
#include "BrokenCore.h"

class lua_State;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingSystems {
public:
	ScriptingSystems();
	~ScriptingSystems();

public:

	// Physics
	void SetMass(float mass);
	float GetMass();

	int GetLinearVelocity(lua_State* L);
	int GetAngularVelocity(lua_State* L);
	void SetLinearVelocity(float x, float y, float z);
	void SetAngularVelocity(float x, float y, float z);

	void AddTorque(float forceX, float forceY, float forceZ, int ForceMode);
	void AddForce(float forceX, float forceY, float forceZ, int ForceMode);

	void SetKinematic(bool enable);
	void UseGravity(bool enable);
	
	void Move(float velX, float velZ);

	// Particles
	void ActivateParticlesEmission() const;
	void DeactivateParticlesEmission() const;

	// Audio
	void PlayAttackSound();
	void PlayStepSound();
	void StopAttackSound();
	void StopStepSound();
	void SetVolume(float volume);

	// Animations
	void StartAnimation(const char* name, float speed);
	void SetAnimSpeed(const char* name, float speed);
	void SetCurrentAnimSpeed(float speed);

	// UI
	void SetBarPercentage(float percentage);
	void SetUIText(const char* text);
	void SetUITextAndNumber(const char* text, float number);
	void SetUITextNumber(float number);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGSYSTEMS_H__