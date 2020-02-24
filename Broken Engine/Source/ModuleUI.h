#pragma once
#include "Module.h"
#include "UI_Element.h"
//#include "Canvas.h"
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
	//void AddCanvas(Canvas* c) { canvas.push_back(c); }

//public:
//	std::vector<Canvas*> canvas;
};

