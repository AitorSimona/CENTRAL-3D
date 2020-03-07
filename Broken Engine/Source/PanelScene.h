#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "BrokenEngine.h"
#include "Imgui/ImGuizmo/ImGuizmo.h"

class PanelScene : public Broken::Panel
{
public:

	PanelScene(char* name);
	~PanelScene();

	bool Draw();

private:
	void HandleGuizmo();

public:
	ImGuizmo::OPERATION guizmoOperation = ImGuizmo::OPERATION::TRANSLATE;
	ImGuizmo::MODE guizmoMode = ImGuizmo::MODE::WORLD;

	float width = 0.0f;
	float height = 0.0f;

	float posX = 0.0f;
	float posY = 0.0f;

	float CurrentSpeedScrollLabel = 1.0f;
};

#endif
