#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "Panel.h"



class PanelScene : public Panel
{
public:

	PanelScene(char* name);
	~PanelScene();

	bool Draw();

	void HandleInput();
	bool SceneHovered = false;


	float width = 0.0f;
	float height = 0.0f;

	float posX = 0.0f;
	float posY = 0.0f;
};

#endif
