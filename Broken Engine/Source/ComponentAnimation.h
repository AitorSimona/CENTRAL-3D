#ifndef __COMPONENT_ANIMATION_H__
#define __COMPONENT_ANIMATION_H__

#include "Component.h"
#include "ResourceAnimation.h"

class ComponentMesh;
class ComponentBone;

struct Animation
{
	Animation(std::string name, uint start, uint end, bool loop, bool Default) : name(name), start(start), end(end), loop(loop), Default(Default) {};

	std::string name;
	uint start;
	uint end;
	bool loop = false;
	bool Default = false;
};


class ComponentAnimation : public Component
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

	void Update(float dt);

	Animation* GetDefaultAnimation() const;

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

public:

	std::vector<Animation*> animations;

	Animation* prev_anim = nullptr;
	Animation* playing_animation = nullptr;;

	ResourceAnimation* res_anim = nullptr;
	float blend_time_value = 0.3f;

	bool draw_bones = false;

private:
	std::vector<Link> links;

	bool linked_channels = false;
	bool linked_bones = false;
	bool created_buffer = false;
	bool has_skeleton = false;
	bool blending = false;

	int Frame = 0;
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


};
#endif // !__COMPONENT_ANIMATION_H__
