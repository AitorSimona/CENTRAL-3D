#include "BrokenCore.h"
#include "Application.h"
#include "GameObject.h"
#include "ModuleUI.h"
#include "ModuleInput.h"
#include "ModuleSceneManager.h"
#include "ModuleGui.h"

#include "Component.h"
#include "ComponentCanvas.h"
#include "ComponentButton.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ModuleUI::ModuleUI(bool start_enabled) : Module(start_enabled)
{
	name = "UI System";
}

ModuleUI::~ModuleUI()
{
}

bool ModuleUI::Init(json& file)
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
		if (obj->HasComponent(Component::ComponentType::Button)) //if has button component
		{
			ComponentButton* element = (ComponentButton*)obj->HasComponent(Component::ComponentType::Button); //single component (change when able to have multiple components of same type)
			element->UpdateCollider(); //update colliders
			element->UpdateState(); //update state
		}
	}
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
	// change camera to ortographic
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, App->gui->sceneHeight, 0, App->gui->sceneWidth, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
	
	// Draw UI
	for (int i = 0; i < canvas.size(); i++)
	{
		if (canvas[i]->visible)
			canvas[i]->Draw();
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
}

void ModuleUI::RemoveCanvas(ComponentCanvas* c)
{
	for (std::vector<ComponentCanvas*>::iterator it = canvas.begin(); it != canvas.end(); ++it)
	{
		if(*it && (*it)->GetContainerGameObject() && (*it)->GetContainerGameObject()->GetUID() == c->GetContainerGameObject()->GetUID())
		{
			canvas.erase(it);
			break;
		}
	}
}

void ModuleUI::Clear()
{
	canvas.clear();
}

bool ModuleUI::CheckMousePos(Component* component, SDL_Rect collider) // 0,0 is bottom left corner
{
	if (App->input->GetKey(SDL_SCANCODE_I) == KEY_DOWN) 
		int i = 0;

	mouse_pos.x = App->input->GetMouseX() - App->gui->sceneX;
	mouse_pos.y = Abs(App->input->GetMouseY() - App->gui->sceneY - App->gui->sceneHeight);

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