#ifndef __ComponentAudioListener__H__
#define __ComponentAudioListener__H__

#include "Component.h"
#include "ModuleAudio.h"
#include "Wwise/AK/SoundEngine/Common/AkTypes.h"

class WwiseGameObject;

class ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(GameObject* ContainerGO);
	ComponentAudioListener();

	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;
	void CreateInspectorNode() override;


	json Save() const override;
	void Load(json& node) override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::AudioListener; };

private:

	WwiseGameObject* wwiseGO = nullptr;
	AkGameObjectID id = 0;
};

#endif __ComponentAudioListener__H__
