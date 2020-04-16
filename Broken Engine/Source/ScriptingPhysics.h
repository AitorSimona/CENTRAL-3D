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
	void SetMass(float mass, uint gameobject_UUID);
	float GetMass(uint gameobject_UUID);

	luabridge::LuaRef GetLinearVelocity(uint gameobject_UUID, lua_State* L);
	luabridge::LuaRef GetAngularVelocity(uint gameobject_UUID, lua_State* L);
	void SetLinearVelocity(float x, float y, float z, uint gameobject_UUID);
	void SetAngularVelocity(float x, float y, float z, uint gameobject_UUID);

	void AddTorque(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID);
	void AddForce(float forceX, float forceY, float forceZ, int ForceMode, uint gameobject_UUID);

	void SetKinematic(bool enable, uint gameobject_UUID);
	void UseGravity(bool enable, uint gameobject_UUID);
	luabridge::LuaRef OverlapSphere(float position_x, float position_y, float position_z, float radius, uint layer, lua_State* L);

	void Move(float vel_x, float vel_z, uint gameobject_UUID);
	luabridge::LuaRef GetCharacterPosition(uint gameobject_UUID, lua_State* L);
	void SetCharacterPosition(float posx, float posy, float posz, uint gameobject_UUID);
	luabridge::LuaRef GetCharacterUpDirection(uint gameobject_UUID, lua_State* L);
	void SetCharacterUpDirection(float rotx, float roty, float rotz, uint gameobject_UUID, lua_State* L);

	int Raycast(float originX, float originY, float originZ, float directionX, float directionY, float directionZ, float maxDistance, int layer, bool hitTriggers);

	int OnTriggerEnter(uint gameobject_UUID);
	int OnTriggerStay(uint gameobject_UUID);
	int OnTriggerExit(uint gameobject_UUID);

	int OnCollisionEnter(uint gameobject_UUID);
	int OnCollisionStay(uint gameobject_UUID);
	int OnCollisionExit(uint gameobject_UUID);
};
BE_END_NAMESPACE
#endif // __SCRIPTINGPHYSICS_H__
