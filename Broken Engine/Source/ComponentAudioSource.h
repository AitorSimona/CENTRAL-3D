#ifndef __COMPONENTAUDIOSOURCE__H__
#define __COMPONENTAUDIOSOURCE__H__

#include "Component.h"
#include "Wwise/AK/SoundEngine/Common/AkTypes.h"

class WwiseGameObject;

class ComponentAudioSource : public Component
{
public:
	ComponentAudioSource(GameObject* ContainerGO);
	virtual ~ComponentAudioSource();

	void CreateInspectorNode() override;
	void Update() override;


	json Save() const override;
	void Load(json& node) override;

	void SetID(uint64 id) { this->id = id; }
	const uint64 GetID() const { return id; }

	static inline Component::ComponentType GetType() { return Component::ComponentType::AudioSource; };

public:

	void PlaySFX() { wwiseGO->PlayEvent(id); }
	void StopSFX() { wwiseGO->StopEvent(id); }
	void PauseSFX() { wwiseGO->PauseEvent(id); }
	void ResumeSFX() { wwiseGO->ResumeEvent(id); }

	void SetVolume(float volume) { wwiseGO->SetVolume(id, volume); }

public:

	WwiseGameObject* wwiseGO = nullptr;
	AkGameObjectID id = 0;
	bool isPaused = false;
	bool isPlaying = false;
};

#endif __COMPONENTAUDIOSOURCE__H__