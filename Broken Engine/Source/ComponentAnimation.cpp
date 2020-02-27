#include "ComponentAnimation.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleTimeManager.h"

#include "GameObject.h"
#include "Imgui/imgui.h"
//#include "OpenGL.h"

#include "mmgr/mmgr.h"

ComponentAnimation::ComponentAnimation(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Animation)
{

}

ComponentAnimation::~ComponentAnimation()
{
	for (int i = 0; i < animations.size(); i++)
	{
		delete animations[i];
	}

	animations.clear();

	if (end_position != nullptr)
	{
		delete[] end_position;
		delete[] end_rotation;
		delete[] end_scale;
		delete[] start_position;
		delete[] start_rotation;
		delete[] start_scale;
	}

	//Free memory from ResourceAnimation
	if (res_anim && res_anim->IsInMemory())
	{
		res_anim->Release();
		res_anim->RemoveUser(GO);
	}

}

void ComponentAnimation::Update(float dt)
{
	if (App->GetAppState() == AppState::PLAY)
	{
		if (linked_channels == false)
		{
			std::vector<GameObject*> childs;
			GO->GetAllChilds(childs);
			has_skeleton = HasSkeleton(childs);

			DoLink();
			playing_animation = CreateAnimation("Idle", 0, 41, true, true);
			CreateAnimation("Run", 0, 42, true);
			CreateAnimation("Punch", 73, 140, false);
		}

		if (linked_bones == false)
			DoBoneLink();

		time += dt;
		if (blending == false)
		{
			UpdateJointsTransform();
		}
		/*else
			BlendAnimations(blend_time_value);*/

		if (has_skeleton)
			UpdateMesh(GO);

		//if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		//{
		//	//prev_anim = playing_animation;
		//	StartBlend(animations[2]->start, animations[2]);
		//	time = 0;
		//}

		//if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) // press key
		//{
		//	//prev_anim = playing_animation;

		//	animations[1]->Default = true;
		//	animations[0]->Default = false;

		//	if (playing_animation->loop)
		//	{
		//		StartBlend(animations[1]->start, animations[1]);
		//		time = 0;
		//	}

		//}
		//if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP) //release key
		//{
		//	animations[1]->Default = false;
		//	animations[0]->Default = true;

		//	if (playing_animation->loop)
		//	{
		//		StartBlend(GetDefaultAnimation()->start, GetDefaultAnimation());
		//		time = 0;
		//	}

		//}
	}
	else
	{
		time = 0;
		if(animations.size()>0)
			playing_animation = GetDefaultAnimation();
	}
	


}

Animation* ComponentAnimation::CreateAnimation(std::string name, uint start, uint end, bool loop, bool Default)
{
	Animation* anim = new Animation(name, start, end, loop, Default);
	animations.push_back(anim);

	return anim;
}

Animation* ComponentAnimation::GetDefaultAnimation() const
{
	for (int i = 0; i < animations.size(); i++)
	{
		if (animations[i]->Default)
		{
			return animations[i];
		}

	}

	//normally first one is idle ?
	return animations[0];
}

json ComponentAnimation::Save() const
{
	json node;

	// --- Store path to component file ---
	if(res_anim)
		node["Resources"]["ResourceAnimation"] = std::string(res_anim->GetResourceFile());
	

	return node;
}

void ComponentAnimation::Load(json& node)
{
	std::string path = node["Resources"]["ResourceAnimation"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	if (res_anim)
		res_anim->Release();

	res_anim = (ResourceAnimation*)App->resources->GetResource(std::stoi(path));

	// --- We want to be notified of any resource event ---
	if (res_anim)
		res_anim->AddUser(GO);
}

void ComponentAnimation::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (res_anim && UID == res_anim->GetUID())
			res_anim = (ResourceAnimation*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (res_anim && UID == res_anim->GetUID())
			res_anim = nullptr;
		break;

	default:
		break;
	}
}

void ComponentAnimation::CreateInspectorNode()
{
	ImGui::Checkbox("##Animator", &GetActive());
	ImGui::SameLine();

	if (ImGui::TreeNode("Animation"))
	{
		ImGui::Checkbox("Draw Bones", &draw_bones);

		ImGui::Text("Im a component animator:D");

		if (res_anim)
			ImGui::Text("Animation: %f", res_anim->GetName());

		

		ImGui::TreePop();
	}
}

void ComponentAnimation::DoLink()
{
	if (res_anim != nullptr)
	{
		std::vector<GameObject*> childs;
		//TODO: Make function GetAllChilds in GameObject.cpp
		GO->GetAllChilds(childs);
		for (int i = 0; i < res_anim->numChannels; i++)
		{
			for (int j = 0; j < childs.size(); j++)
			{
				// We link them if the GO is a bone and their names are equal
				if (childs[j]->GetComponent<ComponentBone>() && childs[j]->GetName().compare(res_anim->channels[i].name) == 0)
				{
					Link lk = Link(childs[j], &res_anim->channels[i]);
					links.push_back(lk);
					break;
				}
			}
		}

		linked_channels = true;
	}
	else
		ENGINE_CONSOLE_LOG("ERROR: Can't link channels, Resource_Animation is nullptr");
}

void ComponentAnimation::DoBoneLink()
{
	std::map<uint, ComponentMesh*> meshes;
	std::vector<ComponentBone*> bones;
	GetAllBones(GO, meshes, bones);

	for (uint i = 0; i < bones.size(); i++)
	{
		// -- Uncomment when Comp/Res Bone is done

		uint tmp_id = bones[i]->res_bone->meshID;
		//They have to have the same ID (Mesh/Bone), that's how they are linked
		std::map<uint, ComponentMesh*>::iterator it = meshes.find(tmp_id);
		if (it != meshes.end())
		{
			it->second->AddBone(bones[i]);
		}
	}

	linked_bones = true;
}

void ComponentAnimation::UpdateJointsTransform()
{
	for (int i = 0; i < links.size(); i++)
	{
		ComponentTransform* trans = links[i].gameObject->GetComponent<ComponentTransform>();

		// ----------------------- Frame count managment -----------------------------------

		Frame = playing_animation->start + (time * res_anim->ticksPerSecond);
		if (Frame == playing_animation->end)
		{
			if (!playing_animation->loop)
				if (playing_animation->Default == false)
				{
					StartBlend(GetDefaultAnimation()->start, GetDefaultAnimation());
				}
			time = 0;
		}
		//-------------------------------------------------------------------------------------
		// POSITION
		float3 position = trans->GetPosition();
		if (links[i].channel->PosHasKey())
		{
			std::map<double, float3>::iterator prev = links[i].channel->PrevPosition(Frame);
			std::map<double, float3>::iterator next = links[i].channel->NextPosition(Frame);

			if (next == links[i].channel->PositionKeys.end())
				next = prev;

			//If both keys are the same, no need to blend
			if (prev == next)
				position = prev->second;
			else
			{
				float value = (Frame - prev->first) / (next->first - prev->first);
				position = prev->second.Lerp(next->second, value);
			}
			
		}
		trans->SetPosition(position);
		//ROTATION
		Quat rotation = trans->GetQuaternionRotation();
		if (links[i].channel->RotHasKey())
		{
			std::map<double, Quat>::iterator prev = links[i].channel->PrevRotation(Frame);
			std::map<double, Quat>::iterator next = links[i].channel->NextRotation(Frame);

			if (next == links[i].channel->RotationKeys.end())
				next = prev;
			//If both keys are the same, no need to blend
			if (prev == next)
				rotation = prev->second;
			else
			{
				float value = (Frame - prev->first) / (next->first - prev->first);
				rotation = prev->second.Slerp(next->second, value);
			}
		}
		trans->SetQuatRotation(rotation);

		//SCALE
		float3 scale = trans->GetScale();
		if (links[i].channel->ScaleHasKey())
		{
			std::map<double, float3>::iterator prev = links[i].channel->PrevScale(Frame);
			std::map<double, float3>::iterator next = links[i].channel->NextScale(Frame);

			if (next == links[i].channel->ScaleKeys.end())
				next = prev;

			std::map<double, float3>::iterator sca = links[i].channel->ScaleKeys.find(Frame);
			if (sca != links[i].channel->ScaleKeys.end())
				scale = sca->second;	
			//If both keys are the same, no need to blend
			if (prev == next)
				scale = prev->second;
			else
			{
				float value = (Frame - prev->first) / (next->first - prev->first);
				scale = prev->second.Lerp(next->second, value);
			}
		}
		trans->Scale(scale.x, scale.y, scale.z);

	}
}

void ComponentAnimation::StartBlend(uint start, Animation* anim)
{
	blend_start_Frame = start;
	next_animation = anim;

	if (end_position == nullptr)
	{
		end_position = new float3[links.size()];
		end_rotation = new Quat[links.size()];
		end_scale = new float3[links.size()];

		start_position = new float3[links.size()];
		start_rotation = new Quat[links.size()];
		start_scale = new float3[links.size()];
	}

	for (int i = 0; i < links.size(); i++)
	{
		ComponentTransform* trans = links[i].gameObject->GetComponent<ComponentTransform>();

		end_position[i] = trans->GetPosition();
		end_rotation[i] = trans->GetQuaternionRotation();
		end_scale[i] = trans->GetScale();
		std::map<double, float3>::iterator next_pos = links[i].channel->PositionKeys.find(blend_start_Frame);
		if (next_pos != links[i].channel->PositionKeys.end())
			start_position[i] = next_pos->second;
		else
			start_position[i] = float3(1234, 0, 0);
		std::map<double, Quat>::iterator next_rot = links[i].channel->NextRotation(blend_start_Frame);
		if (next_rot != links[i].channel->RotationKeys.end())
			start_rotation[i] = next_rot->second;
		else
			start_rotation[i] = Quat(1234, 0, 0, 0);
		std::map<double, float3>::iterator next_sc = links[i].channel->NextScale(blend_start_Frame);
		if (next_sc != links[i].channel->ScaleKeys.end())
			start_scale[i] = next_sc->second;
		else
			start_position[i] = float3(1234, 0, 0);
	}

	blending = true;
}

void ComponentAnimation::BlendAnimations(float blend_time)
{
	curr_blend_time += App->time->GetGameDt();
	float value = curr_blend_time / blend_time;
	for (int i = 0; i < links.size(); i++)
	{
		ComponentTransform* trans = links[i].gameObject->GetComponent<ComponentTransform>();
		if (start_position[i].x != 1234)
			trans->SetPosition(end_position[i].Lerp(start_position[i], value));
		if (start_rotation[i].x != 1234)
			trans->SetQuatRotation(end_rotation[i].Slerp(start_rotation[i], value));
		if (start_scale[i].x != 1234)
			trans->Scale(end_scale[i].Lerp(start_scale[i], value).x, end_scale[i].Lerp(start_scale[i], value).y, end_scale[i].Lerp(start_scale[i], value).z);
	}

	if (curr_blend_time >= blend_time)
	{
		blending = false;
		curr_blend_time = 0;
		playing_animation = next_animation;
	}

}

void ComponentAnimation::UpdateMesh(GameObject* go)
{
	ComponentMesh* tmp = go->GetComponent<ComponentMesh>();

	if (tmp != nullptr)
	{
		tmp->UpdateDefMesh();
		if (tmp->resource_def_mesh != nullptr)
		{
			//Vertex buffer
			if (!created_buffer)
				created_buffer = true;
		}
	}

	for (int i = 0; i < go->childs.size(); i++)
	{
		UpdateMesh(go->childs[i]);
	}
}

void ComponentAnimation::GetAllBones(GameObject* go, std::map<uint, ComponentMesh*>& meshes, std::vector<ComponentBone*>& bones)
{
	ComponentMesh* mesh = go->GetComponent<ComponentMesh>();
	if (mesh != nullptr)
		meshes[mesh->resource_mesh->GetUID()] = mesh;

	ComponentBone* bone = go->GetComponent<ComponentBone>();
	if (bone != nullptr)
		bones.push_back(bone);
	for (uint i = 0; i < go->childs.size(); i++)
		GetAllBones(go->childs[i], meshes, bones);
}

bool ComponentAnimation::HasSkeleton(std::vector<GameObject*> collector) const
{
	for (int i = 0; i < collector.size(); i++)
		if (collector[i]->GetComponent<ComponentBone>());
			return true;

		return false;
}

