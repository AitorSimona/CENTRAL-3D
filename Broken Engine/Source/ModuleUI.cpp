#include "Globals.h"
#include "Application.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"
#include "GameObject.h"

#include "mmgr/mmgr.h"

ModuleUI::ModuleUI(bool start_enabled) : Module(start_enabled)
{
	name = "UI_System";
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init(json file)
{
	return true;
}

bool ModuleUI::Start()
{
	return true;
}

update_status ModuleUI::PreUpdate(float dt)
{
	for (GameObject* obj : App->scene_manager->GetRootGO()->childs) //all objects in scene
	{
		if (obj->HasComponent(Component::ComponentType::UI_Element)) //if has ui component
		{
			//std::vector<Component*> ui = obj->GetAllComponentsOfType(Component::ComponentType::UI_Element); //get all ui components
			//if (!ui.empty())
			//{
				//for (uint i = 0; i < ui.size(); ++i) //update
				//{
					//UI_Element* element = (UI_Element*)ui[i];

					//UI_Element* element = (UI_Element*)obj->GetComponent(Component::ComponentType::UI_Element); //single component (change when able to have multiple components of same type)
					//element->UpdateCollider(); //update colliders
					//element->UpdateState(); //update state

					//if (element->GetState() == UI_Element::State::DRAGGING)
					//{
					//	element->position2D.x = App->input->GetMouseX();
					//	element->position2D.y = App->input->GetMouseY();
					//}
				//}
			//}
		}
	}
	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt)
{
	// CHANGE CAMERA TO ORTOGRAPHIC MODE
	Draw();
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	return true;
}

void ModuleUI::Draw() const
{
	//for (int i = 0; i < canvas.size(); i++)
	//{
	//	if (canvas[i]->visible)
	//		canvas[i]->Draw();
	//}
}
