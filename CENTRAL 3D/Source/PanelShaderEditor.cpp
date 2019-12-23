#include "PanelShaderEditor.h"

#include "Imgui/imgui.h"

#include "Application.h"
#include "ModuleResources.h"
#include "ModuleRenderer3D.h"

#include "ResourceShader.h"

#include "mmgr/mmgr.h"

// https://github.com/ocornut/imgui/issues/2035

static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		std::string* str = (std::string*)data->UserData;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}

	return 1;
}

bool InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CallbackResize;
	return ImGui::InputTextMultiline(label, (char*)str->c_str(), str->capacity() + 1, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags, InputTextCallback, (void*)str);
}

PanelShaderEditor::PanelShaderEditor(char * name) : Panel(name)
{
}

PanelShaderEditor::~PanelShaderEditor()
{

}

bool PanelShaderEditor::Draw()
{
	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar;

	static ImGuiInputTextFlags textflags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize;

	if(currentShader == nullptr)
		currentShader = App->renderer3D->defaultShader;


	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		static ImGuiComboFlags flags = 0;

		ImGui::Text("Shader");
		ImGui::SameLine();

		std::map<std::string, ResourceShader*>* shaders = App->resources->GetShaders();

		const char* item_current = currentShader->name.data();
		if (ImGui::BeginCombo("##Shader", item_current, flags))
		{
			for (std::map<std::string, ResourceShader*>::iterator it = shaders->begin(); it != shaders->end(); ++it)
			{
				bool is_selected = (item_current == it->first);

				if (ImGui::Selectable(it->second->name.data(), is_selected))
				{
					item_current = it->second->name.data();
					currentShader = it->second;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		ImGui::Separator();

		ImGui::Text("Vertex Shader");

		InputText("##VertexShader", &currentShader->vShaderCode, textflags);

		//ImGui::InputTextMultiline("##VertexShader", (char*)VertexData.data(), VertexData.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), textflags);
		
		ImGui::Separator();

		ImGui::Text("Fragment Shader");

		InputText("##FragmentShader", &currentShader->fShaderCode, textflags);


		//ImGui::InputTextMultiline("##FragmentShader", (char*)FragmentData.data(), FragmentData.size(), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), textflags);


		if (ImGui::Button("Compile"))
		{

		}
	}

	ImGui::End();

	return true;
}
