#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "Panel.h"


class PanelProject : public Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();

private:

	void RecursiveDirectoryDraw(const char* directory, const char* filter);
};

#endif
