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



class ModuleEditorUI : public BrokenEngine::Module
{
public:
	ModuleEditorUI(bool start_enabled = true);
	~ModuleEditorUI();

	bool Init(BrokenEngine::json& file) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;


	void SaveStatus(BrokenEngine::json& file) const override;
	void LoadStatus(const BrokenEngine::json& file) override;

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

private:
	std::vector<BrokenEngine::Panel*> panels;
	bool show_demo_window = false;

};

#endif

