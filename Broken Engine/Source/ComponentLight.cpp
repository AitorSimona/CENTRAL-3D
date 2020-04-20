#include "ComponentLight.h"

// -- Modules --
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleSceneManager.h"

// -- Resources --
#include "ResourceShader.h"
#include "ResourceMesh.h"

// -- Components --
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

const std::string GetStringFromLightType(LightType type)
{

	std::string ret = "";
	switch (type)
	{
		case LightType::DIRECTIONAL:		ret = "Directional"; break;
		case LightType::POINTLIGHT:			ret = "Pointlight"; break;
		case LightType::SPOTLIGHT:			ret = "Spotlight"; break;
		case LightType::MAX_LIGHT_TYPES:	ret = "No LightType Defined"; break;
		case LightType::NONE:				ret = "No LightType Defined"; break;
		default:							ret = "No LightType Defined"; break;
	}

	BROKEN_ASSERT((ret != ""), "Unexisting or Unsupported Light Type");
	return ret;
}

ComponentLight::ComponentLight(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Light)
{
	name = "Light";
	App->renderer3D->AddLight(this);
}

ComponentLight::~ComponentLight()
{
	Disable();
	SendUniforms(App->renderer3D->defaultShader->ID, App->renderer3D->GetLightIndex(this));
	App->renderer3D->PopLight(this);
}


void ComponentLight::Update()
{
	ComponentTransform* trans = GetContainerGameObject()->GetComponent<ComponentTransform>();
	if (trans)
	{
		float3 position = float3::zero, scale = float3::one;
		Quat q = Quat::identity;
		trans->GetGlobalTransform().Decompose(position, q, scale);

		if (m_LightType == LightType::DIRECTIONAL)
			m_Direction = position.Normalized();
		else if (m_LightType == LightType::POINTLIGHT || m_LightType == LightType::SPOTLIGHT)
		{
			float3 orientation_vec = float3(2 * (q.x * q.z + q.w * q.y), 2 * (q.y * q.z - q.w * q.x), 1 - 2 * (q.x * q.x + q.y * q.y));
			m_Direction = -orientation_vec;
		}
	}	
	else
		m_Direction = float3(0.0f);
}


void ComponentLight::SendUniforms(uint shaderID, uint lightIndex)
{
	if ((!active || m_LightType == LightType::NONE || m_LightType == LightType::MAX_LIGHT_TYPES) && m_SetToZero)
		return;

	// ------------------ Passing Light Uniforms to Shader ------------------
	// --- Light Uniform name + Index to string ---	
	std::string light_index_str = GetLightUniform(lightIndex, "u_BkLights");

	// --- Uniforms Location Collection ---
	int dirLoc = glGetUniformLocation(shaderID, (light_index_str + ".dir").c_str());
	int posLoc = glGetUniformLocation(shaderID, (light_index_str + ".pos").c_str());
	int colorLoc = glGetUniformLocation(shaderID, (light_index_str + ".color").c_str());
	int intensityLoc = glGetUniformLocation(shaderID, (light_index_str + ".intensity").c_str());
	int attLoc = glGetUniformLocation(shaderID, (light_index_str + ".attenuationKLQ").c_str());
	int cutoffLoc = glGetUniformLocation(shaderID, (light_index_str + ".InOutCutoff").c_str());
	int LtypeLoc = glGetUniformLocation(shaderID, (light_index_str + ".LightType").c_str());

	
	if ((!active || m_LightType == LightType::NONE || m_LightType == LightType::MAX_LIGHT_TYPES) && !m_SetToZero)
	{
		//Set uniforms to 0 and m_SetToZero to true (if the light doesn't has to light)
		// --- Passing Position ---
		glUniform3f(posLoc, 0.0f, 0.0f, 0.0f);
		
		// --- Passing Direction ---
		glUniform3f(dirLoc, 0.0f, 0.0f, 0.0f);

		// --- Passing Color ---
		glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

		// --- Passing Intensity & Light Type
		glUniform1i(LtypeLoc, 0);
		glUniform1f(intensityLoc, 0.0f);

		// --- Passing Light Cutoff
		glUniform2f(cutoffLoc, 0.0f, 0.0f);

		// --- Passing Light Attenuation
		glUniform3f(attLoc, 0.0f, 0.0f, 0.0f);

		m_SetToZero = true;
	}
	else
	{
		//Set uniforms and m_SetToZero to false (if the light has to light)
		// --- Passing Position ---
		ComponentTransform* trans = GetContainerGameObject()->GetComponent<ComponentTransform>();
		float3 pos = float3(0.0f);
		if (trans)
			pos = trans->GetPosition();

		glUniform3f(posLoc, pos.x, pos.y, pos.z);

		// --- Passing Direction ---
		glUniform3f(dirLoc, m_Direction.x, m_Direction.y, m_Direction.z);

		// --- Passing Color ---
		glUniform3f(colorLoc, m_Color.x, m_Color.y, m_Color.z);

		// --- Passing Intensity & Light Type
		glUniform1i(LtypeLoc, (int)m_LightType);
		glUniform1f(intensityLoc, m_Intensity);

		// --- Passing Light Cutoff
		glUniform2f(cutoffLoc, Cos(math::DegToRad(m_InOutCutoffDegrees.x)), Cos(math::DegToRad(m_InOutCutoffDegrees.y)));

		// --- Passing Light Attenuation
		glUniform3f(attLoc, m_AttenuationKLQFactors.x, m_AttenuationKLQFactors.y, m_AttenuationKLQFactors.z);

		if(m_SetToZero)
			m_SetToZero = false;
	}
}

const std::string ComponentLight::GetLightUniform(uint lightIndex, const char* uniformArrayName)
{
	char light_index_chars[10];
	sprintf(light_index_chars, "[%i]", lightIndex);
	return (uniformArrayName + std::string(light_index_chars));
}


// -------------------------------------------------------------------------------------------
void ComponentLight::Draw()
{
	if (!m_DrawMesh)
		return;

	// --- Set Uniforms ---
	uint shaderID = App->renderer3D->defaultShader->ID;
	glUseProgram(shaderID);

	int TextureLocation = glGetUniformLocation(shaderID, "u_UseTextures");
	glUniform1i(TextureLocation, 0);
	GLint vertexColorLocation = glGetUniformLocation(shaderID, "u_Color");
	glUniform4f(vertexColorLocation, m_Color.x, m_Color.y, m_Color.z, 1.0f);

	ComponentTransform* trans = GetContainerGameObject()->GetComponent<ComponentTransform>();
	if (trans)
	{
		GLint modelLoc = glGetUniformLocation(shaderID, "u_Model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, trans->GetGlobalTransform().Transposed().ptr());
	}

	GLint viewLoc = glGetUniformLocation(shaderID, "u_View");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, App->renderer3D->active_camera->GetOpenGLViewMatrix().ptr());

	float nearp = App->renderer3D->active_camera->GetNearPlane();

	// right handed projection matrix
	float f = 1.0f / tan(App->renderer3D->active_camera->GetFOV() * DEGTORAD / 2.0f);
	float4x4 proj_RH(
		f / App->renderer3D->active_camera->GetAspectRatio(), 0.0f, 0.0f, 0.0f,
		0.0f, f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, nearp, 0.0f);

	GLint projectLoc = glGetUniformLocation(shaderID, "u_Proj");
	glUniformMatrix4fv(projectLoc, 1, GL_FALSE, proj_RH.ptr());

	glUniform1i(TextureLocation, 0); //reset texture location

	// --- Draw plane with given texture ---
	glBindVertexArray(App->scene_manager->sphere->VAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, App->scene_manager->sphere->EBO);
	glDrawElements(GL_TRIANGLES, App->scene_manager->sphere->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindVertexArray(0);
}


// -------------------------------------------------------------------------------------------
// --------------------------------------- UI Inspector --------------------------------------
// -------------------------------------------------------------------------------------------
void ComponentLight::CreateInspectorNode()
{

	// --- Type ---
	ImGui::NewLine(); ImGui::Separator();
	static ImGuiComboFlags flags = 0;

	ImGui::Text("Light Type");
	ImGui::SameLine();

	std::string current_type = GetStringFromLightType(m_LightType);
	if (ImGui::BeginCombo("##LightType", current_type.c_str(), flags))
	{
		for (uint i = 0; i < (uint)LightType::MAX_LIGHT_TYPES; ++i)
		{
			std::string selectable_type = GetStringFromLightType(LightType(i));
			bool is_selected = (current_type == selectable_type);

			if (ImGui::Selectable(selectable_type.c_str(), is_selected))
				SetLightType(LightType(i));
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	// --- Draw Mesh ---
	ImGui::Text("Draw Mesh");
	ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 11.0f);
	ImGui::Checkbox("##DrawMesh", &m_DrawMesh);

	// --- Color ---
	ImGui::Separator(); ImGui::NewLine();
	ImGui::Text("Color");
	ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 37.0f);
	ImGui::ColorEdit4("##LightColor", (float*)&m_Color, ImGuiColorEditFlags_NoInputs);		

	// --- Intensity ---
	ImGui::Text("Intensity");
	ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 10.0f);
	ImGui::SetNextItemWidth(300.0f);
	ImGui::SliderFloat("", &m_Intensity, 0.0f, 2.0f);
	ImGui::NewLine();

	if (m_LightType == LightType::SPOTLIGHT)
	{
		// --- Cutoff ---
		ImGui::Separator(); ImGui::NewLine();
		ImGui::Text("Inner Cutoff:"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##InCut", &m_InOutCutoffDegrees.x, 0.1f, 0.00f, m_InOutCutoffDegrees.y - 0.01f);
		ImGui::Text("Outer Cutoff:"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##OutCut", &m_InOutCutoffDegrees.y, 0.01f, m_InOutCutoffDegrees.x + 0.01f, 360.00f);
		ImGui::NewLine();
	}

	if (m_LightType == LightType::SPOTLIGHT || m_LightType == LightType::POINTLIGHT)
	{
		// --- Attenuation ---	
		ImGui::Separator(); ImGui::NewLine();
		ImGui::Text("Constant Attenuation Value (K):"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##AttK", &m_AttenuationKLQFactors.x, 0.001f, 0.000f, 2.0f);
		ImGui::Text("Linear Attenuation Value (L):"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##AttL", &m_AttenuationKLQFactors.y, 0.001f, 0.000f, 2.0f);
		ImGui::Text("Quadratic Attenuation Value (Q):"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##AttQ", &m_AttenuationKLQFactors.z, 0.00001f, 0.000000f, 2.0f, "%.5f");

		if (ImGui::Button("Default", { 57, 18 }))
			m_AttenuationKLQFactors = float3(1.0f, 0.09f, 0.032f);
	}
}


// -------------------------------------------------------------------------------------------
// --------------------------------------- SAVE & LOAD ---------------------------------------
// -------------------------------------------------------------------------------------------
json ComponentLight::Save() const
{
	json node;

	node["Active"] = this->active;
	node["DrawMesh"] = m_DrawMesh;

	node["DirectionX"] = std::to_string(m_Direction.x);
	node["DirectionY"] = std::to_string(m_Direction.y);
	node["DirectionZ"] = std::to_string(m_Direction.z);

	node["ColorR"] = std::to_string(m_Color.x);
	node["ColorG"] = std::to_string(m_Color.y);
	node["ColorB"] = std::to_string(m_Color.z);

	node["AttenuationK"] = std::to_string(m_AttenuationKLQFactors.x);
	node["AttenuationL"] = std::to_string(m_AttenuationKLQFactors.y);
	node["AttenuationQ"] = std::to_string(m_AttenuationKLQFactors.z);

	node["InnerCutoff"] = std::to_string(m_InOutCutoffDegrees.x);
	node["OuterCutoff"] = std::to_string(m_InOutCutoffDegrees.y);

	node["Intensity"] = std::to_string(m_Intensity);
	node["LightType"] = std::to_string((int)m_LightType);


	return node;
}

void ComponentLight::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];
	m_DrawMesh = node["DrawMesh"].is_null() ? true : (bool)node["DrawMesh"];

	// --- Load Strings ---
	std::string str_dirX = node["DirectionX"].is_null() ? "0" : node["DirectionX"];
	std::string str_dirY = node["DirectionY"].is_null() ? "0" : node["DirectionY"];
	std::string str_dirZ = node["DirectionZ"].is_null() ? "0" : node["DirectionZ"];

	std::string str_colR = node["ColorR"].is_null() ? "1" : node["ColorR"];
	std::string str_colG = node["ColorG"].is_null() ? "1" : node["ColorG"];
	std::string str_colB = node["ColorB"].is_null() ? "1" : node["ColorB"];

	std::string str_AttK = node["AttenuationK"].is_null() ? "1" : node["AttenuationK"];
	std::string str_AttL = node["AttenuationL"].is_null() ? "0.09" : node["AttenuationL"];
	std::string str_AttQ = node["AttenuationQ"].is_null() ? "0.032" : node["AttenuationQ"];

	std::string str_inCut = node["InnerCutoff"].is_null() ? "12.5" : node["InnerCutoff"];
	std::string str_outCut = node["OuterCutoff"].is_null() ? "45" : node["OuterCutoff"];

	std::string str_intensity = node["Intensity"].is_null() ? "0.5" : node["Intensity"];
	std::string str_LType = node["LightType"].is_null() ? "1" : node["LightType"];

	// --- Pass Strings to the needed Data Type
	m_Direction = float3(std::stof(str_dirX), std::stof(str_dirY), std::stof(str_dirZ));
	m_Color = float3(std::stof(str_colR), std::stof(str_colG), std::stof(str_colB));

	m_AttenuationKLQFactors = float3(std::stof(str_AttK), std::stof(str_AttL), std::stof(str_AttQ));
	m_InOutCutoffDegrees = float2(std::stof(str_inCut), std::stof(str_outCut));

	m_Intensity = std::stof(str_intensity);
	m_LightType = (LightType)(std::stoi(str_LType));
}