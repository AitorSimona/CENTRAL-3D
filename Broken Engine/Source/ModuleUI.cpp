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
	//for (GameObject* obj : App->scene_manager->GetRootGO()->childs) //all objects in scene
	//{
	//	if (obj->HasComponent(Component::ComponentType::UI_Element)) //if has ui component
	//	{
	//		UI_Element* element = (UI_Element*)obj->GetComponent(Component::ComponentType::UI_Element); //single component (change when able to have multiple components of same type)
	//		element->UpdateCollider(); //update colliders
	//		element->UpdateState(); //update state

	//		if (element->GetState() == DRAGGING)
	//		{
	//			element->position2D.x = App->input->GetMouseX();
	//			element->position2D.y = App->input->GetMouseY();
	//		}
	//	}
	//}
	return UPDATE_CONTINUE;
}

update_status ModuleUI::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleUI::CleanUp()
{
	return true;
}

void ModuleUI::Draw() const
{
	//// change camera to ortographic
	//GLint viewport[4];
	//glGetIntegerv(GL_VIEWPORT, viewport);
	//glMatrixMode(GL_PROJECTION);
	//glPushMatrix();
	//glLoadIdentity();
	//glOrtho(viewport[0], viewport[2], viewport[1], viewport[3], 1, -1);

	//glPushAttrib(GL_LIST_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TRANSFORM_BIT);
	//glMatrixMode(GL_MODELVIEW);
	//glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	//glDisable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Draw UI
	for (int i = 0; i < canvas.size(); i++)
	{
		if (canvas[i]->visible)
			canvas[i]->Draw();
	}

	//glPopAttrib();
	//glPopMatrix();
}

bool ModuleUI::CheckMousePos(Component* component, SDL_Rect collider)
{
	mouse_pos.x = App->input->GetMouseX();
	mouse_pos.y = App->input->GetMouseY();

	//mouse_pos.x -= App->editor->tab_viewport->pos_x + 7;
	//mouse_pos.y = math::Abs(mouse_pos.y - (App->editor->tab_viewport->pos_y + 26 + App->editor->tab_viewport->height));// -mouse_pos.y + App->editor->focused_panel->pos_x;

	SDL_Rect MouseCollider = { mouse_pos.x,mouse_pos.y,1,1 };
	if (SDL_HasIntersection(&MouseCollider, &collider))
		return true;

	return false;
}

bool ModuleUI::CheckClick(Component* component, bool draggable)
{
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		drag_start.x = App->input->GetMouseX();
		drag_start.y = App->input->GetMouseY();
		return true;
	}

	if (draggable && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		return true;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
		return false;

	return false;
}