#include "PanelShaderEditor.h"

#include "OpenGL.h"
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

		//ImGui::SameLine();

		static char shadername[100];

		if (currentShader->name != "Standard")
		{
			if (ImGui::InputText("Rename Shader: ", shadername, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				(*shaders).erase(currentShader->name);
				currentShader->name = shadername;
				App->resources->AddShader(currentShader);
			}
		}


		ImGui::Separator();

		ImGui::Text("Vertex Shader");

		InputText("##VertexShader", &currentShader->vShaderCode, textflags);

		ImGui::Separator();

		ImGui::Text("Fragment Shader");

		InputText("##FragmentShader", &currentShader->fShaderCode, textflags);

		if (ImGui::Button("Compile"))
		{
			currentShader->ReloadAndCompileShader();
			currentShader->GetAllUniforms();
		}

		ImGui::SameLine();

		if (ImGui::Button("New"))
		{
			ResourceShader* new_shader = (ResourceShader*)App->resources->CreateResource(Resource::ResourceType::SHADER);
			new_shader->name = "Shader " + std::to_string(shaders->size() + 1);
			App->resources->AddShader(new_shader);
			currentShader = new_shader;
		}

		ImGui::SameLine();

		if (ImGui::Button("Save All"))
		{
			App->resources->SaveAllShaders();
		}

		ImGui::Separator();
		DisplayAndUpdateUniforms();
	}

	ImGui::End();

	return true;
}

void PanelShaderEditor::DisplayAndUpdateUniforms()
{
	// Note this is being done before any render happens

	static int tmp_int = 0;
	static float tmp_float = 0.0f;
	static float2 tmp_vec2 = { 0.0f,0.0f };
	static float3 tmp_vec3 = { 0.0f,0.0f,0.0f };
	static float4 tmp_vec4 = { 0.0f,0.0f,0.0f,0.0f };

	glUseProgram(currentShader->ID);

	for (uint i = 0; i < currentShader->uniforms.size(); ++i)
	{
		ImGui::Text(currentShader->uniforms[i]->name.data());
		ImGui::SameLine();

		switch (currentShader->uniforms[i]->type)
		{
		case GL_INT:
			if (ImGui::InputInt("##inputintuniform", &tmp_int, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				glUniform1i(currentShader->uniforms[i]->location, tmp_int);
			break;

		case GL_FLOAT:
			if (ImGui::InputFloat("##inputfloatuniform", &tmp_float, 1.0f, 100.0f, "%3f",ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				glUniform1f(currentShader->uniforms[i]->location, tmp_float);
			break;

		case GL_FLOAT_VEC2:
			if (ImGui::InputFloat2("##inputfloat2uniform", tmp_vec2.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				glUniform2f(currentShader->uniforms[i]->location, tmp_vec2.x, tmp_vec2.y);
			break;

		case GL_FLOAT_VEC3:
			if (ImGui::InputFloat3("##inputfloat3uniform", tmp_vec3.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				glUniform3f(currentShader->uniforms[i]->location, tmp_vec3.x, tmp_vec3.y, tmp_vec3.z);
			break;

		case GL_FLOAT_VEC4:
			if (ImGui::InputFloat4("##inputfloat4uniform", tmp_vec4.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
				glUniform4f(currentShader->uniforms[i]->location, tmp_vec4.x, tmp_vec4.y, tmp_vec4.z, tmp_vec4.w);
			break;
		}
	}

	glUseProgram(App->renderer3D->defaultShader->ID);
}


