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
	uint imageSizeX_px = 64;
	uint imageSizeY_px = 64;
	uint item_spacingX_px = 10;
	uint item_spacingY_px = 15;
	uint maxColumns = 10;

	void DrawFolder(ResourceFolder* folder);
	void LimitText( std::string& text);

	void RecursiveDirectoryDraw(const char* directory, std::vector<std::string>& filters);
};

#endif
