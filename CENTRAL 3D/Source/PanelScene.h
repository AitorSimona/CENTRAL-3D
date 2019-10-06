#ifndef __PANEL_SCENE_H__
#define __PANEL_SCENE_H__

#include "Panel.h"

class PanelScene : public Panel
{
public:

	PanelScene(char* name);
	~PanelScene();

	bool Draw();
};

#endif
