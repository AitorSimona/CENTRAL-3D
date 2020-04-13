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
class PanelBuild;
class PanelPhysics;
class PanelNavigation;
class PanelRendering;


class ModuleEditorUI : public Broken::Module
{
public:
	ModuleEditorUI(bool start_enabled = true);
	~ModuleEditorUI();

	bool Init(Broken::json& file) override;
	bool Start() override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;


	const Broken::json& SaveStatus() const override;
	void LoadStatus(const Broken::json& file) override;

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
	PanelBuild*			panelBuild = nullptr;
	PanelPhysics*		panelPhysics = nullptr;
	PanelNavigation*	panelNavigation = nullptr;
	PanelRendering*		panelRendering = nullptr;

private:
	std::vector<Broken::Panel*> panels;
	bool show_demo_window = false;

};

#endif

