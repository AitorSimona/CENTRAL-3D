#ifndef __MODULEUI_H__
#define __MODULEUI_H__
#pragma once

#include "Module.h"
#include "Math.h"

struct SDL_Rect;

BE_BEGIN_NAMESPACE

class Component;
class ComponentCanvas;
class ComponentCamera;

class BROKEN_API ModuleUI : public Module
{
	struct PrioritySort {
		bool operator()(ComponentCanvas* const& node1, ComponentCanvas* const& node2);
	};

public:
	ModuleUI(bool start_enabled = true);
	~ModuleUI();

	bool Init(json& file) override;
	bool Start() override;
	void LoadFont(std::string& font);
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void Draw() const;
	void AddCanvas(ComponentCanvas* c) { canvas.push_back(c); }
	void RemoveCanvas(ComponentCanvas* c);
	
	void Clear();

	bool CheckMousePos(SDL_Rect* collider);
	bool CheckClick(bool draggable = false);

	void OrderCanvas();

public:

	ComponentCamera* ui_camera = nullptr;

	std::vector<ComponentCanvas*> canvas;

	float2 drag_start = float2::zero;
	float2 mouse_pos = float2::zero;

};

BE_END_NAMESPACE
#endif

