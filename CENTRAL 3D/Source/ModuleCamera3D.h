#pragma once
#include "Module.h"
#include "Globals.h"
#include "Math.h"

class GameObject;
class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(json config) override;
	bool Start() override;
	update_status Update(float dt) override;
	void UpdateCamera();
	
	bool CleanUp() override;

	void FrameObject(GameObject* GO);
	void Frame(float3 position);
	void OnMouseClick(const float mouse_x, const float mouse_y);

private:
	void CameraPan(float speed);
	void CameraZoom(float speed);
	void CameraLookAround(float speed, float3 reference);
public:
	float speed = 0.0f;
	float3 reference = { 0.0f,0.0f,0.0f };
	ComponentCamera* camera = nullptr;
	LineSegment last_ray;
};