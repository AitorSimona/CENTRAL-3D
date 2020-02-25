#include "UI_Element.h"
#include "Application.h"
#include "ModuleInput.h"

#include "mmgr/mmgr.h"

UI_Element::UI_Element(GameObject* gameObject, UI_Element::Type Type) : Component(gameObject, Component::ComponentType::UI_Element), type(Type)
{
}

UI_Element::~UI_Element()
{
}

void UI_Element::UpdateCollider()
{
	collider.x = position2D.x - size2D.x;
	collider.y = position2D.y - size2D.y;
	collider.w = size2D.x * 2;
	collider.h = size2D.y * 2;
}

bool UI_Element::CheckMousePos()
{
	UpdateCollider();
	mouse_pos.x = App->input->GetMouseX();
	mouse_pos.y = App->input->GetMouseY();

	//mouse_pos.x -= App->editor->tab_viewport->pos_x + 7;
	//mouse_pos.y = math::Abs(mouse_pos.y - (App->editor->tab_viewport->pos_y + 26 + App->editor->tab_viewport->height));// -mouse_pos.y + App->editor->focused_panel->pos_x;

	SDL_Rect MouseCollider = { mouse_pos.x,mouse_pos.y,1,1 };
	if (SDL_HasIntersection(&MouseCollider, &collider))
		return true;

	return false;
}

bool UI_Element::CheckClick()
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

void UI_Element::UpdateState()
{
	if (interactable && visible)
	{
		if (state != DRAGGING)
		{
			if (CheckMousePos())
			{
				ChangeStateTo(HOVERED);
				if (CheckClick())
				{
					if (draggable && (drag_start.x != mouse_pos.x || drag_start.y != mouse_pos.y))
						ChangeStateTo(DRAGGING);
					else
					{
						if (state != SELECTED)
							DoLogic(action);

						ChangeStateTo(SELECTED);
					}
				}
			}
			else
				ChangeStateTo(IDLE);
		}
		else
		{
			if (!CheckClick())
				ChangeStateTo(IDLE);
		}
	}
}

void UI_Element::DoLogic(Action action)
{
	//switch (action)
	//{
	//case NONE:
	//	break;

	//case SWITCH_VSYNC:
	//	App->renderer3D->SetVSync(!App->renderer3D->GetVSync());
	//	break;

	//case HIDE_MAINMENU:
	//	App->scene->hide_mainmenu = true;
	//	break;
	//}
}