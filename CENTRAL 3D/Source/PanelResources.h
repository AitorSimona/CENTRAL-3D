#ifndef __PANEL_RESOURCES_H__
#define __PANEL_RESOURCES_H__

#include "Panel.h"

class Resource;
struct ImVec4;

class PanelResources : public Panel
{
public:

	PanelResources(char* name);
	~PanelResources();

	bool Draw();

private:
	void DrawResourceNode(Resource* resource, ImVec4& color);
};

#endif
