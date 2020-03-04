#pragma once
#include "Module.h"
#include "ComponentCanvas.h"
#include <vector>

class ModuleUI : public Module
{
public:
	ModuleUI(bool start_enabled = true);
	~ModuleUI();

	bool Init(json file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void Draw() const;
	void AddCanvas(ComponentCanvas* c) { canvas.push_back(c); }

	bool CheckMousePos(Component* element, SDL_Rect collider);
	bool CheckClick(Component* element, bool draggable);

public:
	std::vector<ComponentCanvas*> canvas;

	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;
};

