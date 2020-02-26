#ifndef __COMPONENT_BONE_H__
#define __COMPONENT_BONE_H__

#include "Component.h"
#include "ResourceBone.h"
#include "Math.h"

class GameObject;

class ComponentBone : public Component
{
public:
	ComponentBone(GameObject* gameObject = nullptr);
	virtual ~ComponentBone();

	void DebugDrawBones();

	float4x4 GetBoneTransform() const;
	float4x4 GetSkeletonTransform();
	ComponentBone* GetHipBone();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;
	void CreateInspectorNode() override;

	static inline Component::ComponentType GetType() { return Component::ComponentType::Bone; };


public:
	ResourceBone* res_bone = nullptr;
};

#endif // !__COMPONENT_BONE_H__