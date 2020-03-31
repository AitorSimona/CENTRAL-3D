#include "ComponentAudioSource.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleAudio.h"
#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentAudioSource::ComponentAudioSource( GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::AudioSource)
{
	name = "Audio Source";
	float3 position;
	position.x = ContainerGO->GetComponent<ComponentTransform>()->GetPosition().x;
	position.y = ContainerGO->GetComponent<ComponentTransform>()->GetPosition().y;
	position.z = ContainerGO->GetComponent<ComponentTransform>()->GetPosition().z;

	wwiseGO = wwiseGO->CreateAudioSource(ContainerGO->GetUID(), ContainerGO->GetName(), position);
}

ComponentAudioSource::~ComponentAudioSource() {

	//delete wwiseGO;
	//wwiseGO = nullptr;

	//wwiseGO->~WwiseGameObject();
}

void ComponentAudioSource::CreateInspectorNode()
{
	if (id != 0)
	{
		std::string state = "Pause";

		if (ImGui::ArrowButton("Play", ImGuiDir_Right))
		{
			if (!isPlaying)
			{
				wwiseGO->PlayEvent(id);
				isPlaying = true;
			}
			isPaused = false;
		}

		ImGui::SameLine();


		if (isPaused)
			state = "Resume";

		else
			state = "Pause";

		if (ImGui::Button(state.c_str()))
		{
			if (isPlaying)
			{
				isPlaying = false;
				isPaused = true;
				wwiseGO->PauseEvent(id);
			}
			else if (isPaused)
			{
				isPlaying = true;
				isPaused = false;
				wwiseGO->ResumeEvent(id);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("STOP"))
		{
			isPlaying = false;
			isPaused = false;
			wwiseGO->StopEvent(id);
		}

		if (ImGui::SliderFloat("Volume", &wwiseGO->volume, 0, 10))
		{
			wwiseGO->SetVolume(id, wwiseGO->volume);
		}
	}
}

void ComponentAudioSource::Update()
{
	float3 position;

	position.x = GO->GetComponent<ComponentTransform>()->GetPosition().x;
	position.y = GO->GetComponent<ComponentTransform>()->GetPosition().x;
	position.z = GO->GetComponent<ComponentTransform>()->GetPosition().x;

	wwiseGO->SetPosition(position.x, position.y, position.z);

	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentAudioSource::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];
}

json ComponentAudioSource::Save() const
{
	json node;
	node["Active"] = this->active;
	return node;
}

void ComponentAudioSource::PlaySFX(uint64 id) { wwiseGO->PlayEvent(id); }
void ComponentAudioSource::StopSFX(uint64 id) { wwiseGO->StopEvent(id); }
void ComponentAudioSource::PauseSFX(uint64 id) { wwiseGO->PauseEvent(id); }
void ComponentAudioSource::ResumeSFX(uint64 id) { wwiseGO->ResumeEvent(id); }

void ComponentAudioSource::SetVolume(float volume) { wwiseGO->SetVolume(id, volume); }
