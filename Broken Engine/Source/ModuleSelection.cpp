#include "Application.h"

#include "ModuleSelection.h"
#include "ModuleCamera3D.h"
#include "ModuleEventManager.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"

#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "ResourceScene.h"


using namespace Broken;

ModuleSelection::ModuleSelection(bool start_enabled)
{
	std::string tmp = "root_selection";
	root = new GameObject(tmp.c_str());
	
	name = "Module Selection";

	aabb_selection.SetNegativeInfinity();

	aabb.SetNegativeInfinity();
	aabb = AABB({ 0,0,0 }, { 50,50,50 });
}

ModuleSelection::~ModuleSelection()
{
}

bool ModuleSelection::Init(json& file)
{
	return true;
}

bool ModuleSelection::Start()
{
	return true;
}
bool ModuleSelection::CleanUp()
{
	root->childs.clear();

	delete root;
	root = nullptr;

	return true;
}
update_status ModuleSelection::PreUpdate(float dt)
{
	// Delete selection, it's done through reparenting all selected to selection root object
	// Doing this way avoids problem of ghost parents or childs
	if (!App->scene_manager->go_to_delete.empty())
	{
		root->childs.clear();

		for (int i = 0; i < App->scene_manager->go_to_delete.size(); ++i)
		{
			root->AddChildGO(App->scene_manager->go_to_delete[i]);
		}

		// Actually deleting
		root->RecursiveDelete();
		App->scene_manager->go_count -= App->scene_manager->go_to_delete.size();

		// Cleaning
		ClearSelection();
		App->scene_manager->go_to_delete.clear();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleSelection::Update(float dt)
{
	// SELECTED TODO -> stuck at offset
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		if (is_rectangle_selection)
		{
			aabb_end = float3::zero;
			is_rectangle_selection = false;
		}

		original_scales.clear();
	}

	return UPDATE_CONTINUE;
	if (is_rectangle_selection)
	{
		aabb_end.x += App->input->GetMouseXMotion();
		aabb_end.y -= App->input->GetMouseYMotion();
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			aabb_end.z += 10;
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
			aabb_end.z += 10;

		aabb = AABB( { aabb_start.x,aabb_start.y,aabb_start.z },{ aabb_end.x,aabb_end.y,aabb_end.z });

		float3 frustum_pos = App->renderer3D->active_camera->frustum.Pos() ;
		float4x4 transform = transform.FromTRS(float3(frustum_pos.x, frustum_pos.y, frustum_pos.z),
		App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(),
		float3(1,1, 1));

		aabb.Transform(transform);
		ClearSelection(false);
		SelectIfIntersects();
		//ApplyOBBTransformation();

	}

	
}
update_status ModuleSelection::PostUpdate(float dt)
{
	App->renderer3D->DrawAABB(aabb_selection, { 0.76f, 1, 0.62f,1 });

	return UPDATE_CONTINUE;
	App->renderer3D->DrawOBB(aabb, { 0,0,1,1 });

}

void ModuleSelection::SceneRectangleSelect(float3 start)
{
	aabb = AABB({ start.x,start.y,start.z }, { start.x,start.y,start.z });
	is_rectangle_selection = true;
	aabb_start = start;
	aabb_end = start;
	aabb_end.z += 50;
}

void ModuleSelection::SelectIfIntersects()
{
	if (App->scene_manager->currentScene)
	{
		for (std::unordered_map<uint, GameObject*>::iterator it = App->scene_manager->currentScene->NoStaticGameObjects.begin(); it != App->scene_manager->currentScene->NoStaticGameObjects.end(); it++)
		{
			if ((*it).second->GetUID() != root->GetUID())
			{
				const AABB goaabb = (*it).second->GetAABB();

				if (goaabb.IsFinite() && aabb.Intersects(goaabb))
				{
					Select((*it).second);
				}
			}
		}
		std::vector<GameObject*> static_go;
		App->scene_manager->tree.CollectIntersections(static_go, App->renderer3D->culling_camera->frustum);

		for (std::vector<GameObject*>::iterator it = static_go.begin(); it != static_go.end(); it++)
		{
			if ((*it)->GetUID() != root->GetUID())
			{
				const AABB goaabb = (*it)->GetAABB();

				if (goaabb.IsFinite() && aabb.Intersects(goaabb))
				{
					Select((*it));
				}
			}
		}
	}
	UpdateRoot();

}


void ModuleSelection::ApplyOBBTransformation()
{
	math::vec v = App->camera->camera->frustum.Pos();

	//aabb = AABB({ v.x,v.y,0 }, { v.x + 25,v.y + 25,75 });
	// --- Update transform and rotation to face camera ---
	float3 frustum_pos = App->renderer3D->active_camera->frustum.Pos();
	float3 center = float3(frustum_pos.x, frustum_pos.y, 10);

	// --- Frame image with camera ---
	float4x4 transform = transform.FromTRS(float3(frustum_pos.x, frustum_pos.y, 10),
		App->renderer3D->active_camera->GetOpenGLViewMatrix().RotatePart(),
		float3(1, 1, 1));

	float3 Movement = App->renderer3D->active_camera->frustum.Front();
	float3 camera_pos = frustum_pos;

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		aabb_start.x * 0.01f, aabb_start.y * 0.01f, nearp, 0.0f);

	aabb.Transform(App->camera->camera->GetOpenGLViewMatrix());
	//aabb.Transform(App->camera->camera->GetOpenGLViewMatrix());
}

bool ModuleSelection::IsSelected(const GameObject* gameobject) const
{
	return gameobject != nullptr && gameobject->node_flags & 1;
}

void ModuleSelection::HandleSelection(GameObject* gameobject)
{
	// User is not holding CTRL neither SHIFT -> clean and single select the gameobject
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_IDLE &&
		App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_IDLE)
	{
		ClearSelection(false);
		Select(gameobject);
	}
	// User is holding CTRL, toggle selection state
	else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
	{
		ToggleSelect(gameobject);
	}
	// User is holding SHIFT, multi select the objects between selected and the new one
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
	{
		SelectLastTo(gameobject);
	}

	UpdateRoot();
}

void ModuleSelection::UseGuizmo(ImGuizmo::OPERATION guizmoOperation, ImGuizmo::MODE guizmoMode, float3 pos, float3 rot, float3 scale)
{
	if (guizmoOperation == ImGuizmo::OPERATION::SCALE)// && last_scale.x == 1 && last_scale.y == 1 && last_scale.z == 1)
	{
		for (int i = 0; i < GetSelected()->size(); i++)
			original_scales.push_back(GetSelected()->at(i)->GetComponent<ComponentTransform>()->GetScale());
	}

	for (int i = 0; i < GetSelected()->size(); i++)
	{
		ComponentTransform* t = GetSelected()->at(i)->GetComponent<ComponentTransform>();

		if (guizmoOperation == ImGuizmo::OPERATION::TRANSLATE)
			t->SetPosition(t->GetPosition() + pos);

		else if (guizmoOperation == ImGuizmo::OPERATION::ROTATE)
		{
			if (guizmoMode == ImGuizmo::MODE::LOCAL)
				t->SetRotation(t->GetRotation() + rot);
			/* I made it at first but could not replicate later :(
			else
				t->SetRotation(dummy1);*/
		}
		else if (guizmoOperation == ImGuizmo::OPERATION::SCALE)
		{
			t->Scale(original_scales.at(i).x * scale.x, original_scales.at(i).y * scale.y, original_scales.at(i).z * scale.z);
		}

	}

	UpdateRoot();
}

void ModuleSelection::UpdateRoot()
{
	float3 pos = float3::zero;

	// This part is redone and hardcoded because root has no parent and crashes when accessing the parent globaltransform
	ComponentTransform* root_t = root->GetComponent<ComponentTransform>();
	root_t->Global_transform = float4x4::identity;
	root_t->Local_transform = float4x4::identity;
	root_t->update_transform = true;

	aabb_selection.SetNegativeInfinity();

	for (GameObject* go : *GetSelected())
	{
		ComponentTransform* go_t = go->GetComponent<ComponentTransform>();
		pos += go_t != nullptr ? go_t->position : float3::zero;

		aabb_selection.Enclose(go->GetAABB());
	}
	int size = GetSelected()->size();
	if (size > 0)
		pos /= size;

	root_t->SetPosition(pos);
}

GameObject* ModuleSelection::GetLastSelected()
{
	return GetSelected()->empty() ? nullptr : *GetSelected()->rbegin();
}

// Simple selection -----------------------------------------------
void ModuleSelection::Select(GameObject* gameobject)
{
	if (gameobject == nullptr)
	{
		ClearSelection();
	}
	else if (!IsSelected(gameobject)) {
		gameobject->node_flags |= 1;
		GetSelected()->push_back(gameobject);
		Event e(Event::EventType::GameObject_selected);
		e.go = gameobject;
		App->event_manager->PushEvent(e);
	}
}

void ModuleSelection::UnSelect(GameObject* gameobject)
{
	if (gameobject == nullptr)
	{
		ClearSelection();
	}
	else if(IsSelected(gameobject)){

		for (std::vector<GameObject*>::iterator it = GetSelected()->begin(); it != GetSelected()->end();)
		{
			if ((*it) == gameobject)
			{
				GetSelected()->erase(it);
				gameobject->node_flags &= ~1;
				break;
			}
			it++;
		}
	}
}

// Advanced selection -----------------------------------------------

// returns true if the new state is selected, false otherwise or null
bool ModuleSelection::ToggleSelect(GameObject* gameobject)
{
	if (gameobject == nullptr) return false;

	if (IsSelected(gameobject)) {
		UnSelect(gameobject);
		return false;
	}
	else {
		Select(gameobject);
		return true;
	}
}

void ModuleSelection::SelectLastTo(GameObject* gameobject)
{
	GameObject* from = GetLastSelected();

	if (from != nullptr && gameobject != nullptr) {
		start_selecting = false;
		stop_selecting = false;
		reverse_selecting = false;

		SelectRecursive(App->scene_manager->GetRootGO(), from, gameobject);
	}
}

void ModuleSelection::SelectRecursive(GameObject* gameobject, GameObject* from, GameObject* to)
{
	if (stop_selecting) return;

	if (gameobject->GetUID() == App->scene_manager->GetRootGO()->GetUID())
	{
		if (gameobject->childs.size() > 0)
		{
			for (std::vector<Broken::GameObject*>::iterator it = gameobject->childs.begin(); it != gameobject->childs.end(); ++it)
			{
				SelectRecursive(*it, from, to);
			}
		}
	}

	else
	{
		// Start selecting
		if (gameobject == from) start_selecting = true;
		// End selecting without even start (search from bot to top)
		if (gameobject == to && !start_selecting) {
			start_selecting = true;
			reverse_selecting = true;
		}

		if (start_selecting) Select(gameobject);

		if (reverse_selecting) {
			if (gameobject == from) stop_selecting = true;
		}
		else if (gameobject == to) stop_selecting = true;

		if (gameobject->childs.size() > 0)
		{
			for (std::vector<Broken::GameObject*>::iterator it = gameobject->childs.begin(); it != gameobject->childs.end(); ++it)
			{
				SelectRecursive(*it, from, to);
			}
		}

	}
}

void ModuleSelection::ClearSelection(bool update_root)
{
	for (GameObject* go : *GetSelected())
	{
		go->node_flags &= ~1;
	}
	GetSelected()->clear();

	if (update_root)
		UpdateRoot();
}

// Component Management -----------------------------------------------
bool ModuleSelection::ComponentCanBePasted() const
{
	return (component_type != Component::ComponentType::Unknown);
}
void ModuleSelection::CopyComponentValues(Component* component)
{
	component_type = component->GetType();
	if (component_type != Component::ComponentType::Unknown)
	{
		component_node = component->Save();
		component_name = component->name;
	}
}

void ModuleSelection::PasteComponentValues(Component* component)
{
	if (component_type != Component::ComponentType::Unknown)
		component->Load(component_node);
}

void ModuleSelection::PasteComponentValuesToSelected()
{
	for (GameObject* obj : *GetSelected())
	{
		if (Component * component = obj->HasComponent(component_type))
			component->Load(component_node);
	}
}

void ModuleSelection::DeleteComponentToSelected()
{
	for (GameObject* obj : *GetSelected())
	{
		if (Component * component = obj->HasComponent(component_type))
			component->to_delete = true;
	}

}