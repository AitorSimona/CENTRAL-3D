#include "ModuleSelection.h"
#include "Application.h"
#include "ModuleEventManager.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"



using namespace Broken;

ModuleSelection::ModuleSelection(bool start_enabled)
{
	std::string tmp = "root_selection";
	root = new GameObject(tmp.c_str());
	name = "Module Selection";
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
	// Delete selection
	// Doing this way avoids problem of ghost parents or childs
	if (!App->scene_manager->go_to_delete.empty())
	{
		root->childs.clear();

		for (int i = 0; i < App->scene_manager->go_to_delete.size(); ++i)
		{
			root->AddChildGO(App->scene_manager->go_to_delete[i]);
			//UnSelect(App->scene_manager->go_to_delete[i]);
		}

		root->RecursiveDelete();
		App->scene_manager->go_count -= App->scene_manager->go_to_delete.size();

		ClearSelection();
		App->scene_manager->go_to_delete.clear();
	}

	return UPDATE_CONTINUE;
}

update_status ModuleSelection::Update(float dt)
{
	return UPDATE_CONTINUE;
}

update_status ModuleSelection::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleSelection::IsSelected(GameObject* gameobject)
{
	if (gameobject == nullptr) return false;

	for (int i = 0; i < selection.size(); i++)
	{
		if (selection[i] == gameobject)
			return true;
	}

	return false;
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
		go->node_flags &= ~ImGuiTreeNodeFlags_Selected;
	}
	selection.clear();
}

void ModuleSelection::Select(GameObject* gameobject)
{
	if (gameobject == nullptr)
	{
		ClearSelection();
	}
	else if (!IsSelected(gameobject)) {
		gameobject->node_flags |= ImGuiTreeNodeFlags_Selected;
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

	// --- Display Go node ---
	else
	{
		if (gameobject == from) start_selecting = true;

		if (start_selecting) Select(gameobject);

		if (gameobject == to) stop_selecting = true;

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
				gameobject->node_flags &= ~ImGuiTreeNodeFlags_Selected;
				break;
			}
			it++;
		}

		/*Event e(Event::EventType::GameObject_Unselected);
		e.go = gameobject;
		App->event_manager->PushEvent(e);*/
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