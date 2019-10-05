#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Panel.h"
#include "Imgui/imgui.h"

class PanelConsole : public Panel
{
public:

	PanelConsole(char* name);
	~PanelConsole();

	bool Draw();

private:

	void Clear();
	ImGuiTextFilter filter;
};

#endif

