#pragma once
#include "Module.h"
#include "UI_Element.h"
#include "Canvas.h"
#include <vector>

class ModuleUI : public Module
{
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
	};

public:
	ModuleUI(bool start_enabled = true);
	~ModuleUI();

	bool Init(json file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void Draw() const;
	void AddCanvas(Canvas* c) { canvas.push_back(c); }

	bool CheckMousePos(Component* element);
	bool CheckClick(Component* element);

public:
	std::vector<Canvas*> canvas;

	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;
};

