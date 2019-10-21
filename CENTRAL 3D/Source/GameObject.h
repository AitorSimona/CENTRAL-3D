#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "Globals.h"
#include "Component.h"
#include <list>
#include "Math.h"

class ComponentMaterial;

class GameObject
{

public:

	GameObject(const char* name);
	virtual ~GameObject();

	// --- Getters ---
	uint			GetUID() const;
	std::string		GetName() const;
	float3			GetPosition();
	float3			GetScale();
	float3			GetRotation();
	float4x4        GetLocalTransform();
	Component*		GetComponent(Component::ComponentType type);

	// --- Utilities ---
	Component*		AddComponent(Component::ComponentType type);

	// --- Setters ---
	void			SetPosition(float x, float y, float z);
	void			SetRotationAxisAngle(const float3 &rot_axis, float degrees_angle);
	void			Scale(float x, float y, float z);
	void			SetLocalTransform(float4x4 new_transform);
	void			SetMaterial(ComponentMaterial* material);

private:
	// Unique Identifier
	uint UID = 0;
	float4x4 Local_transform = math::float4x4::identity;
	std::string name;
	std::list<Component*> components;

};

#endif
