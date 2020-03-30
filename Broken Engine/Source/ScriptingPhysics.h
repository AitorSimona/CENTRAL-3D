#ifndef __SCRIPTINGPHYSICS_H__
#define __SCRIPTINGPHYSICS_H__

#include "BrokenCore.h"
#include "MathGeoLib/include/Math/float3.h"

class lua_State;
enum LayerMask;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingPhysics {
public:
	ScriptingPhysics();
	~ScriptingPhysics();

public:
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
	void OverlapSphere(float3 position, float radius, LayerMask layer, lua_State* L);

	int OnTriggerEnter(uint UID, lua_State* L);
	int OnTriggerStay(uint UID, lua_State* L);
	int OnTriggerExit(uint UID, lua_State* L);

	int OnCollisionEnter(uint UID, lua_State* L);
	int OnCollisionStay(uint UID, lua_State* L);
	int OnCollisionExit(uint UID, lua_State* L);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGPHYSICS_H__