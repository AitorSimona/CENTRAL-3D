#include "PanelShaderEditor.h"

// -- Modules --
#include "EngineApplication.h"
#include "ModuleGui.h"
#include "ModuleRenderer3D.h"

// -- Resources --
#include "ResourceMaterial.h"
#include "ResourceShader.h"

// -- Utilities --
#include "Imgui/imgui.h"
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
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoTitleBar;

	static ImGuiInputTextFlags textflags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize;

	if(currentShader == nullptr)
		currentShader = EngineApp->renderer3D->defaultShader;


	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		static ImGuiComboFlags flags = 0;

		ImGui::Text("Shader");
		ImGui::SameLine();

		//std::map<std::string, ResourceShader*>* shaders = EngineApp->resources->GetShaders();

		//const char* item_current = currentShader->name.data();
		//if (ImGui::BeginCombo("##Shader", item_current, flags))
		//{
		//	for (std::map<std::string, ResourceShader*>::iterator it = shaders->begin(); it != shaders->end(); ++it)
		//	{
		//		bool is_selected = (item_current == it->first);

		//		if (ImGui::Selectable(it->second->name.data(), is_selected))
		//		{
		//			item_current = it->second->name.data();
		//			currentShader = it->second;
		//		}
		//		if (is_selected)
		//			ImGui::SetItemDefaultFocus();
		//	}

		//	ImGui::EndCombo();
		//}


		//static char shadername[100];

		//if (currentShader->name != "Standard")
		//{
		//	if (ImGui::InputText("Rename Shader: ", shadername, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		//	{
		//		(*shaders).erase(currentShader->name);
		//		currentShader->name = shadername;
		//		EngineApp->resources->AddShader(currentShader);
		//		EngineApp->scene_manager->SaveScene();
		//	}
		//}


		//ImGui::Separator();

		//ImGui::Text("Vertex Shader");

		//InputText("##VertexShader", &currentShader->vShaderCode, textflags);

		//ImGui::Separator();

		//ImGui::Text("Fragment Shader");

		//InputText("##FragmentShader", &currentShader->fShaderCode, textflags);

		//if (ImGui::Button("Compile"))
		//{
		//	currentShader->ReloadAndCompileShader();

		//	if (EngineApp->scene_manager->GetSelectedGameObject())
		//	{
		//		ComponentMaterial* material = EngineApp->scene_manager->GetSelectedGameObject()->GetComponent<ComponentMaterial>(Component::ComponentType::Material);

		//		// --- Display uniforms ---
		//		if(material)
		//		material->resource_material->shader->GetAllUniforms(material->resource_material->uniforms);
		//	}

		//}

		//ImGui::SameLine();

		//if (ImGui::Button("New"))
		//{
		//	ResourceShader* new_shader = (ResourceShader*)EngineApp->resources->CreateResource(Resource::ResourceType::SHADER);
		//	new_shader->name = "Shader " + std::to_string(shaders->size() + 1);
		//	EngineApp->resources->AddShader(new_shader);
		//	currentShader = new_shader;
		//}

		//ImGui::SameLine();

		//if (ImGui::Button("Save All"))
		//{
		//	EngineApp->resources->SaveAllShaders();
		//}

		ImGui::Separator();
	}
	ImGui::End();



	return true;
}

void PanelShaderEditor::DisplayAndUpdateUniforms(Broken::ResourceMaterial* resource_mat)
{
	// Note this is being done before any render happens

	static int tmp_int = 0;
	static float tmp_float = 0.0f;
	static float2 tmp_vec2 = { 0.0f,0.0f };
	static float3 tmp_vec3 = { 0.0f,0.0f,0.0f };
	static float4 tmp_vec4 = { 0.0f,0.0f,0.0f,0.0f };

	glUseProgram(resource_mat->shader->ID);

	for (uint i = 0; i < resource_mat->uniforms.size(); ++i)
	{
		ImGui::PushID(resource_mat->GetUID() + i);
		ImGui::Text(resource_mat->uniforms[i]->name.data());
		ImGui::SameLine();

		switch (resource_mat->uniforms[i]->type)
		{
		case GL_INT:
			tmp_int = resource_mat->uniforms[i]->value.intU;

			if (ImGui::InputInt("##inputintuniform", &tmp_int, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.intU = tmp_int;
			}
			break;

		case GL_FLOAT:
			tmp_float = resource_mat->uniforms[i]->value.floatU;

			if (ImGui::InputFloat("##inputfloatuniform", &tmp_float, 1.0f, 100.0f, "%3f", ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.floatU = tmp_float;
			}
			
			break;

		case GL_FLOAT_VEC2:
			tmp_vec2 = resource_mat->uniforms[i]->value.vec2U;

			if (ImGui::InputFloat2("##inputfloat2uniform", tmp_vec2.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.vec2U = tmp_vec2;
			}
			
			break;

		case GL_FLOAT_VEC3:
			tmp_vec3 = resource_mat->uniforms[i]->value.vec3U;

			if (ImGui::InputFloat3("##inputfloat3uniform", tmp_vec3.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.vec3U = tmp_vec3;
			}
			
			break;

		case GL_FLOAT_VEC4:
			tmp_vec4 = resource_mat->uniforms[i]->value.vec4U;

			if (ImGui::InputFloat4("##inputfloat4uniform", tmp_vec4.ptr(), 2, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.vec4U = tmp_vec4;
			}
			
			break;

		case GL_INT_VEC2:
			tmp_vec2 = resource_mat->uniforms[i]->value.vec2U;

			if (ImGui::InputInt2("##inputint2uniform", (int*)tmp_vec2.ptr(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.vec2U = tmp_vec2;
			}
			break;

		case GL_INT_VEC3:
			tmp_vec3 = resource_mat->uniforms[i]->value.vec3U;

			if (ImGui::InputInt3("##inputint3uniform", (int*)tmp_vec3.ptr(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.vec3U = tmp_vec3;
			}
			break;

		case GL_INT_VEC4:
			tmp_vec4 = resource_mat->uniforms[i]->value.vec4U;

			if (ImGui::InputInt4("##inputint4uniform", (int*)tmp_vec4.ptr(), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				resource_mat->uniforms[i]->value.vec4U = tmp_vec4;
			}

			break;
		}

		ImGui::PopID();
	}

	glUseProgram(EngineApp->renderer3D->defaultShader->ID);
}


