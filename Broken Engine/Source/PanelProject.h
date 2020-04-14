#ifndef __PANEL_PROJECT_H__
#define __PANEL_PROJECT_H__

#include "Panel.h"

#include <string>
#include <vector>

namespace Broken
{
	class GameObject;
	class Resource;
	class ResourceFolder;
	struct Event;
}

typedef unsigned int uint;
struct ImVec4;
struct ImVec2;

class PanelProject : public Panel
{
public:

	PanelProject(char* name);
	~PanelProject();

	bool Draw();
	void SetSelected(Broken::Resource* new_selected);
	Broken::Resource* GetSelected();
	//const Broken::Resource* GetcurrentDirectory() const;
	void GatherGameObjects(Broken::GameObject* go, std::vector<Broken::GameObject*>& gos_vec);

private:

	static void ONGameObjectSelected(const Broken::Event& e);
	static void ONResourceDestroyed(const Broken::Event& e);
	void CreateResourceHandlingPopup();
private:

	bool wasclicked = false;
	uint imageSize_px = 48;
	uint item_spacingX_px = 20;
	uint item_spacingY_px = 15;
	uint maxColumns = 10;

	Broken::Resource* selected = nullptr;
	uint selected_uid = 0;
	Broken::ResourceFolder* currentDirectory = nullptr;

	void DrawFolder(Broken::ResourceFolder* folder);
	void DrawFile(Broken::Resource* resource, uint i, uint row, ImVec2& cursor_pos, ImVec4& color, bool child = false);
	void LimitText( std::string& text);

	void RecursiveDirectoryDraw(Broken::ResourceFolder* directory);

	bool createScript = false;
};

#endif
