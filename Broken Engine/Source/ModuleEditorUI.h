#ifndef __MODULEEDITORUI_H__
#define __MODULEEDITORUI_H__

#include "BrokenEngine.h"

class PanelSettings;
class PanelAbout;
class PanelConsole;
class PanelInspector;
class PanelHierarchy;
class PanelScene;
class PanelToolbar;
class PanelProject;
class PanelShaderEditor;
class PanelResources;

class ModuleEditorUI : public BrokenEngine::Module
{
public:
	ModuleEditorUI(bool start_enabled = true);
	~ModuleEditorUI();

	bool Init(BrokenEngine::json& file) override;
	update_status Update(float dt) override;

public:

	PanelSettings*		panelSettings = nullptr;
	PanelAbout*			panelAbout = nullptr;
	PanelConsole*		panelConsole = nullptr;
	PanelInspector*		panelInspector = nullptr;
	PanelHierarchy*		panelHierarchy = nullptr;
	PanelScene*			panelScene = nullptr;
	PanelToolbar*       panelToolbar = nullptr;
	PanelProject*		panelProject = nullptr;
	PanelShaderEditor*  panelShaderEditor = nullptr;
	PanelResources*		panelResources = nullptr;

private:
	bool show_demo_window = false;

};

#endif

