#pragma once
#include "Component.h"
#include "Math.h"
#include "Color.h"

#define NUM_ACTIONS 3

class UI_Element : public Component
{
	friend class ModuleGUI;

public:
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
	State state = NOTHING;
	Action action = NONE;

	SDL_Rect collider = { 0,0,0,0 };

	const char* action_list[NUM_ACTIONS] = {
	"None",
	"Switch VSync",
	"Hide Main Menu"
	};

};