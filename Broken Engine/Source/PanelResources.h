#ifndef __PANEL_RESOURCES_H__
#define __PANEL_RESOURCES_H__

#include "BrokenEngine.h"

struct ImVec4;

class PanelResources : public Broken::Panel
{
public:

	PanelResources(char* name);
	~PanelResources();

	bool Draw();

private:
	void DrawResourceNode(Broken::Resource* resource, ImVec4& color);
};

#endif
