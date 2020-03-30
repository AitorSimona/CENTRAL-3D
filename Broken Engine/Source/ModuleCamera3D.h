#ifndef __BE_MODULECAMERA3D_H__
#define __BE_MODULECAMERA3D_H__

#include "Module.h"
#include "Math.h"
#include "BrokenCore.h"


BE_BEGIN_NAMESPACE

class GameObject;
class ComponentCamera;
class ComponentAudioListener;

class BROKEN_API ModuleCamera3D : public Module {
public:

	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Init(json& file) override;
	bool Start() override;
	update_status Update(float dt) override;
	void UpdateCamera();

	bool CleanUp() override;

	void LoadStatus(const json& file) override;

	void FrameObject(GameObject* GO);
	void FrameObject(float3 posToLook);
	void OnMouseClick(const float mouse_x, const float mouse_y);

	const float4 GetCameraDefaultValues() const { return m_CameraDefaultValues; }

private:

	void CameraPan(float speed);
	void CameraZoom(float speed);
	void CameraLookAround(float speed, float3 reference);

	//Modify engine's camera speed (for the mouse mid button scroll)
	void ModifySpeedMultiplicator();

public:

	float m_CameraSpeed = 10.0f;
	float m_ScrollSpeed = 3.0f;
	float m_SpeedMultiplicator = 1.0f;
	bool m_ScrollingSpeedChange = false;

	float3 reference = { 0.0f,0.0f,0.0f };
	ComponentCamera* camera = nullptr;
	ComponentAudioListener* listener = nullptr;
	LineSegment last_ray;

	float4 m_CustomDefaultCameraValues = float4(60.0f, 0.1f, 1000.0f, 1.6f);

private:

	float m_CameraSpeedDeltaTime = 0.0f;
	float m_ScrollSpeedDeltaTime = 0.0f;
	float m_FinalSpeed = 0.0f;


	float4 m_CameraDefaultValues = float4(60.0f, 0.1f, 1000.0f, 1.6f);
};

BE_END_NAMESPACE
#endif