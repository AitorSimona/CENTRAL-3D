#include "ModuleSelection.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ComponentCamera.h"
#include "ResourceScene.h"
#include "ModuleEventManager.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"
#include "ModuleGui.h"
#include "ModuleRenderer3D.h"
#include "GameObject.h"

using namespace Broken;

ModuleSelection::ModuleSelection(bool start_enabled)
{
	std::string tmp = "root_selection";
	root = new GameObject(tmp.c_str());
	name = "Module Selection";

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
	delete root;
	root = nullptr;

	selection.clear();

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

	return UPDATE_CONTINUE;
	if (aabb_selection)
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
		ClearSelection();
		SelectIfIntersects();
		//ApplyOBBTransformation();

	}

	if (aabb_selection && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
	{
		aabb_end = float3::zero;
		aabb_selection = false;
	}
}
update_status ModuleSelection::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
	App->renderer3D->DrawOBB(aabb, { 0,1,0,1 });

}

void ModuleSelection::SceneRectangleSelect(float3 start)
{
	aabb = AABB({ start.x,start.y,start.z }, { start.x,start.y,start.z });
	aabb_selection = true;
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
bool ModuleSelection::IsSelected(GameObject* gameobject)
{
	if (gameobject == nullptr) return false;

	return gameobject->node_flags & 1;
}

void ModuleSelection::HandleSelection(GameObject* gameobject) 
{
	// User is not holding CTRL neither SHIFT -> clean and single select the gameobject
	if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_IDLE &&
		App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_IDLE && App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_IDLE)
	{
		ClearSelection();
		Select(gameobject);
	}
	// User is holding CTRL, toggle selection state
	else if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)
	{
		ToggleSelect(gameobject);
	}
	// SELECTED TODO
	// User is holding SHIFT, multi select the objects between selected and the new one
	else if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || App->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT)
	{
		SelectLastTo(gameobject);
	}
}

GameObject* ModuleSelection::GetLastSelected() const
{
	return selection.empty() ? nullptr : *selection.rbegin();
}


void ModuleSelection::ClearSelection()
{
	for (GameObject* go : selection)
	{
		go->node_flags &= ~1;
	}
	selection.clear();
}
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
	for (GameObject* obj : selection)
	{
		if (Component* component = obj->HasComponent(component_type))
			component->Load(component_node);
	}
}

void ModuleSelection::DeleteComponentToSelected()
{
	for (GameObject* obj : selection)
	{
		if (Component * component = obj->HasComponent(component_type))
			component->to_delete = true;
	}

}
void ModuleSelection::Select(GameObject* gameobject)
{
	if (gameobject == nullptr)
	{
		ClearSelection();
	}
	else if (!IsSelected(gameobject)) {
		gameobject->node_flags |= 1;
		selection.push_back(gameobject);
		Event e(Event::EventType::GameObject_selected);
		e.go = gameobject;
		App->event_manager->PushEvent(e);
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
				SelectRecursive(*it,from, to);
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
				SelectRecursive(*it,from,to);
			}
		}

	}
}


void ModuleSelection::UnSelect(GameObject* gameobject)
{
	if (gameobject == nullptr)
	{
		ClearSelection();
	}
	else if(IsSelected(gameobject)){

		for (std::vector<GameObject*>::iterator it = selection.begin(); it != selection.end();)
		{
			if ((*it) == gameobject)
			{
				selection.erase(it);
				gameobject->node_flags &= ~1;
				break;
			}
			it++;
		}
	}
}



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