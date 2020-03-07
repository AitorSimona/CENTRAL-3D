#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "BrokenEngine.h"
#include "Imgui/imgui.h"

class PanelConsole : public Broken::Panel
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

