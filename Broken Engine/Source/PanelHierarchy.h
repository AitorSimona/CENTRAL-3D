#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "BrokenEngine.h"

class PanelHierarchy : public BrokenEngine::Panel
{
public:

	PanelHierarchy(char* name);
	~PanelHierarchy();

	bool Draw();

private:
	void DrawRecursive(BrokenEngine::GameObject* Go);

	BrokenEngine::GameObject* to_destroy = nullptr;
	bool end_drag = false;
	BrokenEngine::GameObject* target = nullptr;
	BrokenEngine::GameObject* dragged = nullptr;
};

#endif
