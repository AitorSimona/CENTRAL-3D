#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "Panel.h"
#include "Globals.h"
#include <string>
#include <vector>

class ResourceFolder;

class PanelProject : public Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();

	std::string dragged;
private:
	uint imageSize_x = 40;
	uint imageSize_y = 40;
	uint item_spacing_x = 10;
	uint item_spacing_y = 0;

	void DrawFolder(ResourceFolder* folder);
	void LimitText( std::string& text);

	void RecursiveDirectoryDraw(const char* directory, std::vector<std::string>& filters);
};

#endif
