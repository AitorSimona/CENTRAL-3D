#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Imgui/imgui.h"
#include "Panel.h"

class PanelConsole : public Panel
{
public:

	PanelConsole(char* name);
	~PanelConsole();

	bool Draw();

private:

	void Clear();
	ImGuiTextFilter filter;
	char* error_key = "|";
	char* warning_key = "!";
};

#endif

