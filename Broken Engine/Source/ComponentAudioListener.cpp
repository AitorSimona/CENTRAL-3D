#include "ComponentAudioListener.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleAudio.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include "ModuleSceneManager.h"
#include "Imgui/imgui.h"


using namespace Broken;

ComponentAudioListener::ComponentAudioListener(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::AudioListener)
{

	float3 position;
	position.x = ContainerGO->GetComponent<ComponentTransform>()->GetPosition().x;
	position.y = ContainerGO->GetComponent<ComponentTransform>()->GetPosition().y;
	position.z = ContainerGO->GetComponent<ComponentTransform>()->GetPosition().z;

	wwiseGO = wwiseGO->CreateAudioListener(ContainerGO->GetUID(), ContainerGO->GetName(), position);
}

ComponentAudioListener::~ComponentAudioListener() {}

void ComponentAudioListener::CreateInspectorNode()
{
	if (ImGui::CollapsingHeader("Audio Listener", ImGuiTreeNodeFlags_Leaf))
	{
		if (ImGui::Button("Delete component"))
			to_delete = true;
	}
}

void ComponentAudioListener::Update()
{
	ComponentCamera* cam = App->camera->camera;
	float3 pos = cam->frustum.Pos();//.pos;
	float3 front = cam->frustum.Front();//front;
	float3 top = cam->frustum.Up();//up;

	wwiseGO->SetPosition(pos.x, pos.y, pos.z, front.x, front.y, front.z, top.x, top.y, top.z);

	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentAudioListener::Load(json& node)
{
}

json ComponentAudioListener::Save() const
{
	json node;
	return node;
}