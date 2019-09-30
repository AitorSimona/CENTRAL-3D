#ifndef __MODULE_GUI_H__
#define __MODULE_GUI_H__

#include "Module.h"

#include <vector>

class Panel;
class PanelSettings;
class PanelAbout;

class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Init(json file);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Draw() const;
	void DockSpace() const;
	void RequestBrowser(const char * url) const;
	//bool LoadEditorConfig() const;

	void LogFPS(float fps, float ms);

	void SaveStatus(json file) const override;

	void LoadStatus(const json file) override;

public:

	PanelSettings*		panelSettings = nullptr;
	PanelAbout*			panelAbout = nullptr;

private:
	bool show_demo_window = false;

	std::vector<Panel*> panels;
};

#endif
