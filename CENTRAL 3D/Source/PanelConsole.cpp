#include "Globals.h"
#include "PanelConsole.h"
#include "Application.h"

#include "mmgr/mmgr.h"

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

		ImGui::SameLine();

		filter.Draw("Filter", 200.0f);

		ImGui::Separator();

		ImGuiWindowFlags scrollFlags = 0;
		scrollFlags |= ImGuiWindowFlags_HorizontalScrollbar;
		scrollFlags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;

		if (ImGui::BeginChild("Scrollbar", ImVec2(0, 0), false, scrollFlags))
		{
			// --- Print logs to console ---

			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(2, 1)); // Tighten spacing

			for (uint i = 0; i < App->GetLogs().size(); ++i)
			{
				const char* item = App->GetLogs().at(i).data();

				// --- Display error messages in red color ---
				if(item[1] == *error_key)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.75, 0, 0, 255));
				else
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 255, 255, 255));

				// --- If text does not match the filter don't print it ---
				if (!filter.PassFilter(item))
				{
					ImGui::PopStyleColor();
					continue;
				}

				ImGui::TextUnformatted(item);
				ImGui::PopStyleColor();

			}

			ImGui::PopStyleVar();

		}

		if(ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();

	}

	ImGui::End();

	return true;
}

void PanelConsole::Clear()
{
	App->ClearLogsFromConsole();
}
