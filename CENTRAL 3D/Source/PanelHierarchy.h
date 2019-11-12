#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"

class GameObject;

class PanelHierarchy : public Panel
{
public:

	PanelHierarchy(char* name);
	~PanelHierarchy();

	bool Draw();

private:
	void DrawRecursive(GameObject* Go);

	GameObject* to_destroy = nullptr;
	bool end_drag = false;
	GameObject* target = nullptr;
	GameObject* dragged = nullptr;
};

#endif
