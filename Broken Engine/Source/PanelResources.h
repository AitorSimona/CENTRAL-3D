#ifndef __PANEL_RESOURCES_H__
#define __PANEL_RESOURCES_H__

#include "Panel.h"

struct ImVec4;

namespace Broken
{
	class Resource;
}

class PanelResources : public Panel
{
public:

	PanelResources(char* name);
	~PanelResources();

	bool Draw();

private:
	void DrawResourceNode(Broken::Resource* resource, ImVec4& color);
};

#endif
