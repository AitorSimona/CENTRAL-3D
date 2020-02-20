#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "Panel.h"
#include "Globals.h"
#include <string>
#include <vector>

class ResourceFolder;
class Resource;
struct Event;

class PanelProject : public Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();
	void SetSelected(Resource* new_selected);
private:
	static void ONGameObjectSelected(const Event& e);
	static void ONResourceDestroyed(const Event& e);
private:
	uint imageSize_px = 48;
	uint item_spacingX_px = 10;
	uint item_spacingY_px = 15;
	uint maxColumns = 10;

	Resource* selected = nullptr;
	uint selected_uid = 0;
	ResourceFolder* currentDirectory = nullptr;

	void DrawFolder(ResourceFolder* folder);
	void LimitText( std::string& text);

	void RecursiveDirectoryDraw(const char* directory, std::vector<std::string>& filters);
};

#endif
