#ifndef __ComponentAudioSource__H__
#define __ComponentAudioSource__H__

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

	void SetID(AkGameObjectID id);

	static inline Component::ComponentType GetType() { return Component::ComponentType::AudioSource; };

public:

	WwiseGameObject* wwiseGO = nullptr;
	AkGameObjectID id = 0;
	bool isPaused = false;
	bool isPlaying = false;
};

#endif __ComponentAudioSource__H__