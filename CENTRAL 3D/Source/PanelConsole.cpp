#include "PanelConsole.h"

PanelConsole::PanelConsole(char * name) : Panel(name)
{
}

PanelConsole::~PanelConsole()
{
	Clear();
}

bool PanelConsole::Draw()
{

	ImGuiWindowFlags consoleFlags = 0;
	consoleFlags |= ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, consoleFlags))
	{
		if (ImGui::SmallButton("Clear"))
		{
			Clear();
		}

	}

	ImGui::End();

	return true;
}

void PanelConsole::AddLog(const char * log)
{
}

void PanelConsole::Clear()
{
}
