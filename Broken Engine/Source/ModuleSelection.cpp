#include "ModuleSelection.h"
#include "Application.h"
#include "ModuleEventManager.h"
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

	//hierarchy_order.clear();
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
	else {
		gameobject->node_flags |= ImGuiTreeNodeFlags_Selected;
		selection.push_back(gameobject);
		Event e(Event::EventType::GameObject_selected);
		e.go = gameobject;
		App->event_manager->PushEvent(e);
	}
}

//void ModuleSelection::SelectLastTo(GameObject* gameobject)
//{
//	GameObject* from = GetLastSelected();
//
//	if ( from != nullptr && gameobject != nullptr) {
//
//		for (GameObject* go : hierarchy_order)
//		{
//			if (go == from) start_selecting = true;
//			
//			if (start_selecting) Select(go);
//
//			if (go == gameobject) return;
//		}
//	}
//}


void ModuleSelection::UnSelect(GameObject* gameobject)
{
	if (gameobject == nullptr)
	{
		ClearSelection();
	}
	else {
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