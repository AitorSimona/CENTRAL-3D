#pragma once
#include "Component.h"
#include "Math.h"
#include "Color.h"

#define NUM_ACTIONS 3

class UI_Element : public Component
{
	friend class ModuleGUI;

public:
	enum Type
	{
		UNKNOWN = 0,
		CANVAS,
		IMAGE,
		TEXT,
		BUTTON,
		CHECKBOX,
		INPUTTEXT,
		PROGRESSBAR
	};

	enum State
	{
		NOTHING = 0,
		IDLE,
		HOVERED,
		SELECTED,
		LOCKED,
		DRAGGING
	};

	enum Action
	{
		NONE = 0,
		SWITCH_VSYNC,
		HIDE_MAINMENU
	};

public:
	UI_Element(GameObject* gameObject, UI_Element::Type type);
	virtual ~UI_Element();

	UI_Element::Type GetType() const { return type; }
	UI_Element::State GetState() { return state; }
	UI_Element::Action GetAction() const { return action; }

	void ChangeActionTo(Action new_action) { action = new_action; }
	void ChangeStateTo(State new_state) { state = new_state; }
	void ChangeColor(Color new_color) { color = new_color; }
	void ChangeSize(float2 size) { size2D = size; }
	void ChangePosition(float2 pos) { position2D = pos; }
	void ChangeRotation(float rot) { rotation2D = rot; }

	void UpdateCollider();
	void UpdateState();
	void DoLogic(Action action);

	virtual void Draw() const {};

	//Load & Save
	virtual json Save() const = 0;
	virtual void Load(json& node) = 0;
	virtual void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) {};
	virtual void CreateInspectorNode() = 0;

private:
	bool CheckMousePos();
	bool CheckClick();

public:
	bool visible = true;
	bool draggable = true;
	bool interactable = true;

	float2 size2D = { 50,50 };
	float2 position2D = { 50,50 };
	float rotation2D = 0.0f;

	Color color = White;

protected:
	Type type = UNKNOWN;
	State state = NOTHING;
	Action action = NONE;

	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;

	SDL_Rect collider = { 0,0,0,0 };

	const char* action_list[NUM_ACTIONS] = {
	"None",
	"Switch VSync",
	"Hide Main Menu"
	};

};