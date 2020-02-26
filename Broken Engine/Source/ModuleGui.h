#ifndef __MODULE_GUI_H__
#define __MODULE_GUI_H__

#include "Module.h"
#include "Panel.h"
#include "BrokenCore.h"

#include <vector>
//
//class PanelSettings;
//class PanelAbout;
//class PanelConsole;
//class PanelInspector;
//class PanelHierarchy;
//class PanelScene;
//class PanelToolbar;
//class PanelProject;
//class PanelShaderEditor;
//class PanelResources;

BE_BEGIN_NAMESPACE

class ModuleGui : public Module
{
public:

	ModuleGui(bool start_enabled = true);
	~ModuleGui();

	bool Init(json file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void Draw() const;
	void DockSpace() const;
	void RequestBrowser(const char * url) const;
	void AddPanel(Panel* npanel);

	void LogFPS(float fps, float ms);

	void SaveStatus(json &file) const override;
	void LoadStatus(const json & file) override;

	void HandleInput(SDL_Event* event) const;

	bool IsKeyboardCaptured() const;
	bool IsMouseCaptured() const;

	void CreateIcons();

public:

	/*PanelSettings*	panelSettings = nullptr;
	PanelAbout*			panelAbout = nullptr;
	PanelConsole*		panelConsole = nullptr;
	PanelInspector*		panelInspector = nullptr;
	PanelHierarchy*		panelHierarchy = nullptr;
	PanelScene*			panelScene = nullptr;
	PanelToolbar*       panelToolbar = nullptr;
	PanelProject*		panelProject = nullptr;
	PanelShaderEditor*  panelShaderEditor = nullptr;
	PanelResources*		panelResources = nullptr;*/
	
	uint materialTexID = 0;
	uint folderTexID = 0;
	uint defaultfileTexID = 0;
	uint prefabTexID = 0;
	uint playbuttonTexID = 0;
	uint sceneTexID = 0;
private:
	bool capture_keyboard = false;
	bool capture_mouse = false;

	std::vector<BrokenEngine::Panel*> panels;
};

BE_END_NAMESPACE
#endif
