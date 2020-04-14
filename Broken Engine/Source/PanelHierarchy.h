#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"
namespace Broken
{
	class GameObject;
}

class PanelHierarchy : public Panel
{
public:

	PanelHierarchy(char* name);
	~PanelHierarchy();

	bool Draw();
	void ExitEditPrefab();

private:
	void DrawRecursive(Broken::GameObject* Go);

	bool wasclicked = false;
	bool end_drag = false;
	bool to_unparent = false;
	unsigned int selected_uid = 0;
	Broken::GameObject* target = nullptr;
	Broken::GameObject* dragged = nullptr;
};

#endif
