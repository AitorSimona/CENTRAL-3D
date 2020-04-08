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

	bool openPrefab = false;
	ResourcePrefab* prefab = nullptr;
private:
	GameObject* prefabParent = nullptr;
	bool editingPrefab = false;
	void DrawRecursive(GameObject* Go);
};

#endif
