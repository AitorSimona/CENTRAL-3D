#ifndef __PANEL_RESOURCES_H__
#define __PANEL_RESOURCES_H__

#include "BrokenEngine.h"

struct ImVec4;

class PanelResources : public BrokenEngine::Panel
{
public:

	PanelResources(char* name);
	~PanelResources();

	bool Draw();

private:
	void DrawResourceNode(BrokenEngine::Resource* resource, ImVec4& color);
};

#endif
