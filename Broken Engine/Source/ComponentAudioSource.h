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

	void PlaySFX(uint64 id);
	void StopSFX(uint64 id);
	void PauseSFX(uint64 id);
	void ResumeSFX(uint64 id);

	void SetVolume(float volume);

public:

	WwiseGameObject* wwiseGO = nullptr;
	AkGameObjectID id = 0;
	bool isPaused = false;
	bool isPlaying = false;
};

#endif __COMPONENTAUDIOSOURCE__H__