#include "ComponentAnimation.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentBone.h"
#include "ComponentMesh.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleTimeManager.h"
#include "ModuleInput.h"
#include "ResourceAnimation.h"
#include "ResourceAnimator.h"
#include "ImporterAnimator.h"

#include "GameObject.h"
#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ComponentAnimation::ComponentAnimation(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Animation)
{
	name = "Animation";
}

ComponentAnimation::~ComponentAnimation()
{
	for (int i = 0; i < animations.size(); i++)
	{
		if (animations[i])
		{
			delete animations[i];
			animations[i] = nullptr;
		}
		
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

	if (res_animator && res_animator->IsInMemory())
	{
		res_animator->Release();
		res_animator->RemoveUser(GO);
	}

}

void ComponentAnimation::Update()
{
	if (linked_channels == false)
	{
		std::vector<GameObject*> childs;
		GO->GetAllChilds(childs);
		has_skeleton = HasSkeleton(childs);

		DoLink();
	}

	if (App->GetAppState() == AppState::PLAY && !App->time->gamePaused)
	{
		if (linked_bones == false)
			DoBoneLink();

		time += App->time->GetGameDt();

		if (animations.size() > 0)
		{
			if (blending == false)
				UpdateJointsTransform();
			else
				BlendAnimations(blend_time_value);

			if (has_skeleton)
				UpdateMesh(GO);
		}
	}
	else
	{
		time = 0;
	}

	if (to_copy)
	{
		// -- New Copy function goes here, or in the button itself

		//ENGINE_AND_SYSTEM_CONSOLE_LOG("Animation info size: %d", anim_info.size());
		

		to_copy = false;
	}

	if(to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

Animation* ComponentAnimation::CreateAnimation(std::string name, uint start, uint end, bool loop, bool Default)
{
	Animation* anim = new Animation(name, start, end, loop, Default);
	if (res_anim != nullptr)
		anim->speed = res_anim->ticksPerSecond;
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

	return nullptr;
}

void ComponentAnimation::PlayAnimation(const char* name, float speed)
{
	for (int i = 0; i < animations.size(); ++i)
	{
		if (animations[i] == nullptr)
		{
			ENGINE_AND_SYSTEM_CONSOLE_LOG("Create an animation to play it!");
			break;
		}
		else if (animations[i]->name.compare(name) == 0)
		{
			animations[i]->speed = speed;
			StartBlend(animations[i]);
			time = 0;
		}
	}
}

void ComponentAnimation::ChangeBlendTime(float value)
{
	if(value > 0)
		blend_time_value = value;
	else
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Can't change blend time, value has to be greater than 0");
}

void ComponentAnimation::SetAnimationSpeed(const char* name, float speed)
{
	for (int i = 0; i < animations.size(); ++i)
	{
		if (animations[i] == nullptr)
		{
			ENGINE_AND_SYSTEM_CONSOLE_LOG("No animations at index %i", i);
			break;
		}
		else if (animations[i]->name.compare(name) == 0)
			animations[i]->speed = speed;
	}
}

void ComponentAnimation::SetCurrentAnimationSpeed(float speed)
{
	if(playing_animation)
		playing_animation->speed = speed;
	else
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Current Animation is nullptr!");
}

json ComponentAnimation::Save() const
{
	json node;
	node["Active"] = this->active;


	node["Resources"]["ResourceAnimation"];

	// --- Store path to component file ---
	if (res_anim)
		node["Resources"]["ResourceAnimation"] = std::string(res_anim->GetResourceFile());
	
	if (!res_animator)
	{
		// --- Saving animations ------------------
		node["Animations"]["Size"] = std::to_string(animations.size());
		node["Animations"]["BlendTime"] = std::to_string(blend_time_value);
		node["Animations"]["UseDefault"] = use_default_animation;

		for (int i = 0; i < animations.size(); ++i)
		{
			std::string iterator = std::to_string(i);
			node["Animations"][iterator]["Name"] = animations[i]->name;
			node["Animations"][iterator]["Start"] = std::to_string(animations[i]->start);
			node["Animations"][iterator]["End"] = std::to_string(animations[i]->end);
			node["Animations"][iterator]["Loop"] = animations[i]->loop;
			node["Animations"][iterator]["Default"] = animations[i]->Default;
		}
	}
	else
	{
		node["Resources"]["ResourceAnimator"] = std::to_string(res_animator->GetUID());
	}
	

	return node;
}

void ComponentAnimation::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];

	// Load Animation resource ------------------------------------------------------------------------------------------
	std::string path = node["Resources"]["ResourceAnimation"].is_null() ? "0" : node["Resources"]["ResourceAnimation"];
	App->fs->SplitFilePath(path.c_str(), nullptr, &path);
	path = path.substr(0, path.find_last_of("."));

	if (res_anim)
		res_anim->Release();

	res_anim = (ResourceAnimation*)App->resources->GetResource(std::stoi(path));

	// --- We want to be notified of any resource event ---
	if (res_anim)
		res_anim->AddUser(GO);
	//-------------------------------------------------------------------------------------------------------------------


	// Load Animator resource -------------------------------------------------------------------------------------------
	std::string res_uid = node["Resources"]["ResourceAnimator"].is_null() ? "0" : node["Resources"]["ResourceAnimator"];
	int uid = std::stoi(res_uid);

	LoadAnimator(true, uid);

	//--------------------------------------------------------------------------------------------------------------------

	if (!res_animator)
	{
		//--- Loading animations ---

		std::string size = node["Animations"]["Size"].is_null() ? "0" : node["Animations"]["Size"];
		int anim_size = std::stoi(size);

		std::string blend_time = node["Animations"]["BlendTime"].is_null() ? "0" : node["Animations"]["BlendTime"];
		blend_time_value = std::stof(blend_time);

		use_default_animation = node["Animations"]["UseDefault"].is_null() ? false : (bool)node["Animations"]["UseDefault"];

		for (int i = 0; i < anim_size; ++i)
		{
			std::string iterator = std::to_string(i);
			std::string name = node["Animations"][iterator]["Name"].is_null() ? "" : node["Animations"][iterator]["Name"];
			std::string start = node["Animations"][iterator]["Start"].is_null() ? "" : node["Animations"][iterator]["Start"];
			std::string end = node["Animations"][iterator]["End"].is_null() ? "" : node["Animations"][iterator]["End"];
			bool loop = node["Animations"][iterator]["Loop"].is_null() ? false : (bool)node["Animations"][iterator]["Loop"];
			bool Default = node["Animations"][iterator]["Default"].is_null() ? false : (bool)node["Animations"][iterator]["Default"];

			CreateAnimation(name, std::stoi(start), std::stoi(end), loop, Default);

		}
	}
	

}

void ComponentAnimation::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if (res_animator && UID == res_animator->GetUID())
			LoadAnimator(false);
			//res_anim = (ResourceAnimation*)App->resources->GetResource(UID);
		break;

	case Resource::ResourceNotificationType::Deletion:
		if (res_anim && UID == res_anim->GetUID())
			res_anim = nullptr;

		if (res_animator && UID == res_animator->GetUID())
			res_animator = nullptr;
		break;

	default:
		break;
	}
}

void ComponentAnimation::CreateInspectorNode()
{
	if (res_anim)
	{

		if (ImGui::Button("Delete component"))
			to_delete = true;

		if (res_anim)
		{
			ImGui::Text("Animation name: %s", res_anim->name.c_str());
			ImGui::PushItemWidth(50); ImGui::InputFloat("Blend Duration", &blend_time_value);
			ImGui::Checkbox("Draw Bones", &draw_bones);
			ImGui::Checkbox("Use Default Animation", &use_default_animation);

			if (ImGui::Button("Create New Animation"))
				CreateAnimation("New Animation", 0, 0, false);


			if (res_animator)
			{
				if (ImGui::Button("Save animation info"))
				{

					res_animator->FreeMemory();

					for (auto iterator = animations.begin(); iterator != animations.end(); ++iterator)
					{
						Animation* anim = new Animation((*iterator)->name, (*iterator)->start, (*iterator)->end, (*iterator)->loop, (*iterator)->Default);
						res_animator->animations.push_back(anim);
					}

					ImporterAnimator* IAnim = App->resources->GetImporter<ImporterAnimator>();
					IAnim->Save(res_animator);
				}
			}
			else
				ImGui::Text("No Animator applied");


			ImGui::SameLine();
			ImGui::ImageButton(NULL, ImVec2(20, 20), ImVec2(0, 0), ImVec2(1, 1), 2);

			// --- Handle drag & drop ---
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
				{
					uint UID = *(const uint*)payload->Data;
					Resource* resource = App->resources->GetResource(UID, false);

					if (resource && resource->GetType() == Resource::ResourceType::ANIMATOR)
					{
						LoadAnimator(true, UID);
					}
				}

				ImGui::EndDragDropTarget();
			}

			for (int i = 0; i < animations.size(); i++)
			{
				ImGui::Separator();
				// --- Game Object Name Setter ---
				char Anim_name[100] = "";
				strcpy_s(Anim_name, 100, animations[i]->name.c_str());
				std::string str = "Animation ";
				ImGui::PushItemWidth(200); if (ImGui::InputText(str.append(std::to_string(i + 1)).c_str(), Anim_name, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
					animations[i]->name = Anim_name;

				ImGui::Text("Animation Frames: %i", (animations[i]->end - animations[i]->start));
				std::string Start = animations[i]->name;
				ImGui::PushItemWidth(100); ImGui::InputInt(Start.append(" Start").c_str(), &animations[i]->start, 1, 0);
				std::string End = animations[i]->name;
				ImGui::PushItemWidth(100); ImGui::InputInt(End.append(" End").c_str(), &animations[i]->end, 1, 0);
				std::string Speed = animations[i]->name;
				ImGui::PushItemWidth(100); ImGui::InputFloat(Speed.append(" Speed").c_str(), &animations[i]->speed, 1, 0);
				std::string Loop = animations[i]->name;
				ImGui::Checkbox(Loop.append(" Loop").c_str(), &animations[i]->loop);

				std::string name1 = animations[i]->name;
				std::string Preview = "Preview ";
				Preview = Preview.append(name1);
				if (ImGui::Button(Preview.c_str()))
				{
					if (App->GetAppState() == AppState::PLAY)
						PlayAnimation(name1.c_str(), animations[i]->speed);
					else
						ENGINE_CONSOLE_LOG("To preview an animation game has to be running!");
				}


				std::string Delete = "Delete ";
				std::string button = Delete.append(name1);
				if (ImGui::Button(button.c_str()))
				{
					delete animations[i];
					animations.erase(animations.begin() + i);
				}
			}
		}
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
				if (childs[j]->GetComponent<ComponentBone>() && std::string(childs[j]->GetName()).compare(res_anim->channels[i].name) == 0)
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

		if (bones[i]->res_bone)
		{
			uint tmp_id = bones[i]->res_bone->meshID;
			//They have to have the same ID (Mesh/Bone), that's how they are linked
			std::map<uint, ComponentMesh*>::iterator it = meshes.find(tmp_id);
			if (it != meshes.end())
			{
				it->second->AddBone(bones[i]);
			}
		}
	}

	linked_bones = true;
}

void ComponentAnimation::UpdateJointsTransform()
{
	
	for (int i = 0; i < links.size() && playing_animation != nullptr; i++)
	{
		ComponentTransform* trans = links[i].gameObject->GetComponent<ComponentTransform>();

		bool update_transforms = true;
		// ----------------------- Frame count managment -----------------------------------
		Frame = playing_animation->start + (time * playing_animation->speed);
		if (Frame >= playing_animation->end)
		{
			if (use_default_animation)
			{
				if (!playing_animation->loop)
					if (playing_animation->Default == false)
					{
						StartBlend(GetDefaultAnimation());
					}
				time = 0;
			}
			else
			{
				if (playing_animation->loop)
					time = 0;
				else
					update_transforms = false;

			}
		}

		if (update_transforms)
		{
			//-------------------------------------------------------------------------------------
			// POSITION
			float3 position = trans->GetPosition();
			if (links[i].channel->PosHasKey())
			{

				std::map<double, float3>::iterator pos = links[i].channel->PositionKeys.find(Frame);
				if (pos != links[i].channel->PositionKeys.end())
					position = pos->second;
				else
				{
					//Blend prev with next
					std::map<double, float3>::iterator prev = links[i].channel->PrevPosition(Frame);
					std::map<double, float3>::iterator next = links[i].channel->NextPosition(Frame);

					if (next == links[i].channel->PositionKeys.end())
						next = prev;
					else
					{
						float value = (Frame - prev->first) / (next->first - prev->first);
						position = prev->second.Lerp(next->second, value);
					}
				}

			}
			trans->SetPosition(position);
			//ROTATION
			Quat rotation = trans->GetQuaternionRotation();
			if (links[i].channel->RotHasKey())
			{
				std::map<double, Quat>::iterator rot = links[i].channel->RotationKeys.find(Frame);
				if (rot != links[i].channel->RotationKeys.end())
					rotation = rot->second;
				else
				{
					//Blend prev with next
					std::map<double, Quat>::iterator prev = links[i].channel->PrevRotation(Frame);
					std::map<double, Quat>::iterator next = links[i].channel->NextRotation(Frame);

					if (next == links[i].channel->RotationKeys.end())
						next = prev;
					else
					{
						float value = (Frame - prev->first) / (next->first - prev->first);
						rotation = prev->second.Slerp(next->second, value);
					}
				}
			}
			trans->SetQuatRotation(rotation);

			//SCALE
			float3 scale = trans->GetScale();
			if (links[i].channel->ScaleHasKey())
			{
				std::map<double, float3>::iterator sca = links[i].channel->ScaleKeys.find(Frame);
				if (sca != links[i].channel->ScaleKeys.end())
					scale = sca->second;
				else
				{
					//Blend prev with next
					std::map<double, float3>::iterator prev = links[i].channel->PrevScale(Frame);
					std::map<double, float3>::iterator next = links[i].channel->NextScale(Frame);

					if (next == links[i].channel->ScaleKeys.end())
						next = prev;
					else
					{
						float value = (Frame - prev->first) / (next->first - prev->first);
						scale = prev->second.Lerp(next->second, value);
					}
				}
			}
			trans->Scale(scale.x, scale.y, scale.z);

		}
	}	
}

void ComponentAnimation::StartBlend(Animation* anim)
{
	blend_start_Frame = anim->start;
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
			start_scale[i] = float3(1234, 0, 0);
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
		time = 0;
	}

}

void ComponentAnimation::UpdateMesh(GameObject* go)
{
	ComponentMesh* tmp = go->GetComponent<ComponentMesh>();

	if (tmp != nullptr && tmp->bones.size()>0)
	{
		tmp->UpdateDefMesh();
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
	{
		if(mesh->resource_mesh)
		meshes[mesh->resource_mesh->GetUID()] = mesh;
	}

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

void ComponentAnimation::LoadAnimator(bool drop, uint UID)
{
	for (auto it = animations.begin(); it != animations.end(); ++it)
	{
		if (res_animator)
		{
			bool found = false;
			for (auto iterator = res_animator->animations.begin(); iterator != res_animator->animations.end(); ++iterator)
			{

				if ((*it) == (*iterator))
				{
					found = true;
				}
			}

			if (!found)
			{
				delete (*it);
				(*it) = nullptr;
			}

		}
		else
		{
			delete (*it);
			(*it) = nullptr;
		}
	}

	if (drop && res_animator)
	{
		res_animator->Release();
		res_animator->RemoveUser(GO);
	}

	animations.clear();

	if (drop)
	{
		res_animator = (ResourceAnimator*)App->resources->GetResource(UID);
		if(res_animator)
			res_animator->AddUser(GO);
	}
	
	if(res_animator)
		for (auto it = res_animator->animations.begin(); it != res_animator->animations.end(); ++it)
		{
			CreateAnimation((*it)->name, (*it)->start, (*it)->end, (*it)->loop, (*it)->Default);
		}
}
