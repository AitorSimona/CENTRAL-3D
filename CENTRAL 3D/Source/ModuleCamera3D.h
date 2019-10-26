#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class GameObject;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void FrameObject(GameObject& GO);

private:
	void CameraPan(float speed);
	void CameraZoom(float speed);
	void CameraOrbit();
	void CameraLookAround();
	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
};