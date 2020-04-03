#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "Panel.h"
#include "Globals.h"
#include <string>
#include <vector>

class ResourceFolder;
class Resource;
struct Event;
struct ImVec2;
struct ImVec4;

class PanelProject : public Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();
	void SetSelected(Resource* new_selected);
	const Resource* GetcurrentDirectory() const;
	Resource* GetSelected();

private:
	static void ONGameObjectSelected(const Event& e);
	static void ONResourceDestroyed(const Event& e);
	void CreateResourceHandlingPopup();

private:
	bool wasclicked = false;
	uint imageSize_px = 48;
	uint item_spacingX_px = 20;
	uint item_spacingY_px = 15;
	uint maxColumns = 10;

	Resource* selected = nullptr;
	uint selected_uid = 0;
	ResourceFolder* currentDirectory = nullptr;

	void DrawFolder(ResourceFolder* folder);
	void DrawFile(Resource* resource, uint i, uint row, ImVec2& cursor_pos, ImVec4& color, bool child = false);
	void LimitText(std::string& text);

	void RecursiveDirectoryDraw(ResourceFolder* folder);
};

#endif
