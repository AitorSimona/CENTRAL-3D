#ifndef __SCRIPTINGPHYSICS_H__
#define __SCRIPTINGPHYSICS_H__

#include "BrokenCore.h"
#include "MathGeoLib/include/Math/float3.h"

class lua_State;
enum LayerMask;

namespace luabridge {
	class LuaRef;
}

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
	luabridge::LuaRef OverlapSphere(float position_x, float position_y, float position_z, float radius, uint layer, lua_State* L);

	void Move(float vel_x, float vel_y);
	int GetCharacterPosition(lua_State* L);
	void SetCharacterPosition(float posx, float posy, float posz);
	int GetCharacterUpDirection(lua_State* L);
	void SetCharacterUpDirection(float rotx, float roty, float rotz, lua_State* L);

	int OnTriggerEnter(uint UID, lua_State* L);
	int OnTriggerStay(uint UID, lua_State* L);
	int OnTriggerExit(uint UID, lua_State* L);

	int OnCollisionEnter(uint UID, lua_State* L);
	int OnCollisionStay(uint UID, lua_State* L);
	int OnCollisionExit(uint UID, lua_State* L);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGPHYSICS_H__
