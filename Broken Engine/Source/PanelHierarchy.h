#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "BrokenEngine.h"

class PanelHierarchy : public Broken::Panel
{
public:

	PanelHierarchy(char* name);
	~PanelHierarchy();

	bool Draw();
	void ExitEditPrefab();

private:
	void DrawRecursive(Broken::GameObject* Go);

	bool wasclicked = false;
	uint selected_uid = 0;
	bool end_drag = false;
	Broken::GameObject* target = nullptr;
	Broken::GameObject* dragged = nullptr;
};

#endif
