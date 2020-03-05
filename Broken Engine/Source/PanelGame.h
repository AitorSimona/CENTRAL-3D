#ifndef __PANELGAME_H__
#define __PANELGAME_H__
#pragma once


#include "Panel.h"
class PanelGame : public Panel {
public:
	PanelGame(char* name);
	~PanelGame();

	bool Draw();

public:
	bool SceneHovered = false;

	float width = 0.0f;
	float height = 0.0f;

	float posX = 0.0f;
	float posY = 0.0f;
};


#endif

