#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "Panel.h"
#include <string>

class PanelProject : public Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();

	std::string dragged;
private:

	void RecursiveDirectoryDraw(const char* directory, const char* filter);
};

#endif
