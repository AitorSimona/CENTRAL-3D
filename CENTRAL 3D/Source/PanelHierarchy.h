#ifndef __PANEL_HIERARCHY_H__
#define __PANEL_HIERARCHY_H__

#include "Panel.h"

class GameObject;
class ResourcePrefab;

class PanelHierarchy : public Panel
{
public:

	PanelHierarchy(char* name);
	~PanelHierarchy();

	bool Draw();

	void ExitEditPrefab();

	bool openPrefab = false;
	bool editingPrefab = false;
	ResourcePrefab* prefab = nullptr;
private:
	void DrawRecursive(GameObject* Go);
};

#endif
