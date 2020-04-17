#ifndef __COMPONENT_ANIMATION_H__
#define __COMPONENT_ANIMATION_H__

#include "Component.h"
#include "Math.h"

BE_BEGIN_NAMESPACE

class ComponentMesh;
class ComponentBone;
class ResourceAnimation;
class ResourceAnimator;
class Channel;

struct BROKEN_API Animation
{
	Animation(std::string name, uint start, uint end, bool loop, bool Default) : name(name), start(start), end(end), loop(loop), Default(Default) {};

	std::string name;
	int start = 0;
	int end = 0;
	bool loop = false;
	bool Default = false;
	float speed = 30;
};


class BROKEN_API ComponentAnimation : public Component
{
	struct Link //Stores a bone(gameobject) with his corresponding channel of the animation
	{
		Link(GameObject* gameObject, Channel* channel) : gameObject(gameObject), channel(channel) {};
		GameObject* gameObject;
		Channel* channel;
	};

public:
	ComponentAnimation(GameObject* ContainerGO);
	virtual ~ComponentAnimation();

	void Update() override;

	Animation* GetDefaultAnimation() const;

	// --- Scripting ---
	void PlayAnimation(const char* name, float speed = 30);
	void ChangeBlendTime(float value);
	void SetAnimationSpeed(const char* name, float speed);
	void SetCurrentAnimationSpeed(float speed);
	bool CurrentAnimationEnded();

	// --- Save & Load ---
	json Save() const override;
	void Load(json& node) override;
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;
	void CreateInspectorNode();

	static inline Component::ComponentType GetType() { return Component::ComponentType::Animation; };

private:
	Animation* CreateAnimation(std::string name, uint start, uint end, bool loop, bool Default = false);
	void DoLink();
	void DoBoneLink();
	void UpdateJointsTransform();

	void UpdateMesh(GameObject* go);
	void GetAllBones(GameObject* go, std::map<uint, ComponentMesh*>& meshes, std::vector<ComponentBone*>& bones);

	void BlendAnimations(float blend_time);
	void StartBlend(Animation* anim);

	bool HasSkeleton(std::vector<GameObject*> GO) const;

	void LoadAnimator(bool drop, uint UID = 0);

public:

	std::vector<Animation*> animations;

	Animation* playing_animation = nullptr;;

	ResourceAnimation* res_anim = nullptr;
	ResourceAnimator* res_animator = nullptr;
	float blend_time_value = 0.3f;

	bool draw_bones = false;
	bool use_default_animation = false;

private:
	std::vector<Link> links;

	bool linked_channels = false;
	bool linked_bones = false;
	bool created_buffer = false;
	bool has_skeleton = false;
	bool blending = false;

	float Frame = 0;
	float time = 0;

	//---- blend vars ------------
	float curr_blend_time = 0;
	uint blend_start_Frame = 0;

	float3* end_position = nullptr;
	Quat* end_rotation = nullptr;
	float3* end_scale = nullptr;

	float3* start_position = nullptr;
	Quat* start_rotation = nullptr;
	float3* start_scale = nullptr;
	Animation* next_animation = nullptr;

	bool to_copy = false;
};

BE_END_NAMESPACE
#endif // !__COMPONENT_ANIMATION_H__
