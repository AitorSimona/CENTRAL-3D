#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "BrokenEngine.h"
#include <string>
#include <vector>
#include "Imgui/imgui.h"


class PanelProject : public BrokenEngine::Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();
	void SetSelected(BrokenEngine::Resource* new_selected);
	//const BrokenEngine::Resource* GetcurrentDirectory() const;

private:
	static void ONGameObjectSelected(const BrokenEngine::Event& e);
	static void ONResourceDestroyed(const BrokenEngine::Event& e);
private:
	bool wasclicked = false;
	uint imageSize_px = 48;
	uint item_spacingX_px = 20;
	uint item_spacingY_px = 15;
	uint maxColumns = 10;

	BrokenEngine::Resource* selected = nullptr;
	uint selected_uid = 0;
	//BrokenEngine::ResourceFolder* currentDirectory = nullptr;

	void DrawFolder(BrokenEngine::ResourceFolder* folder);
	void DrawFile(BrokenEngine::Resource* resource, uint i, uint row, ImVec2& cursor_pos, ImVec4& color, bool child = false);
	void LimitText( std::string& text);

	void RecursiveDirectoryDraw(const char* directory, std::vector<std::string>& filters);
};

#endif
