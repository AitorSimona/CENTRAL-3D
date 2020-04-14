#ifndef __ComponentAudioListener__H__
#define __ComponentAudioListener__H__

#include "Component.h"

BE_BEGIN_NAMESPACE

class WwiseGameObject;

class BROKEN_API ComponentAudioListener : public Component
{
public:
	ComponentAudioListener(GameObject* ContainerGO);
	virtual ~ComponentAudioListener();

	void CreateInspectorNode() override;
	void Update() override;

	json Save() const override;
	void Load(json& node) override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::AudioListener; };

private:

	WwiseGameObject* wwiseGO = nullptr;
	unsigned __int64 id = 0;
};

BE_END_NAMESPACE
#endif __ComponentAudioListener__H__
