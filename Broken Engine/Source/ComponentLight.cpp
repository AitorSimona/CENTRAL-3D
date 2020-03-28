#include "ComponentLight.h"
//#include "ComponentTransform.h"
//#include "Application.h"
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

}

ComponentLight::~ComponentLight()
{
}

void ComponentLight::Update()
{
}

json ComponentLight::Save() const
{
	json node;

	node["Active"] = this->active;

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
	m_Direction =				float3(std::stof(str_dirX), std::stof(str_dirY), std::stof(str_dirZ));
	m_Color =					float3(std::stof(str_colR), std::stof(str_colG), std::stof(str_colB));

	m_AttenuationKLQFactors =	float3(std::stof(str_AttK), std::stof(str_AttL), std::stof(str_AttQ));
	m_InOutCutoffDegrees =		float2(std::stof(str_inCut), std::stof(str_outCut));

	m_Intensity =	std::stof(str_intensity);
	m_LightType =	(LightType)(std::stoi(str_LType));
}

void ComponentLight::CreateInspectorNode()
{
	// --- Basic ---
	ImGui::Checkbox("##Light Component", &GetActive());
	ImGui::SameLine();

	// --- Tree Begin ---
	if (ImGui::TreeNode("Light Component"))
	{
		// --- Delete ---
		if (ImGui::Button("Delete component"))
			to_delete = true;

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

		// --- Color ---
		ImGui::Separator(); ImGui::NewLine();
		ImGui::Text("Color");
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 37.0f);
		ImGui::ColorEdit4("##LightColor", (float*)&m_Color, ImGuiColorEditFlags_NoInputs);		

		// --- Intensity ---
		ImGui::Text("Intensity");
		ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x + 10.0f);
		ImGui::SetNextItemWidth(300.0f);
		ImGui::SliderFloat("", &m_Intensity, 0.0f, 2.0f, "%.3f", 1.0f);
		ImGui::NewLine();

		// --- Cutoff ---
		ImGui::Separator(); ImGui::NewLine();
		ImGui::Text("Inner Cutoff:"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##InCut", &m_InOutCutoffDegrees.x, 0.1f, 0.00f, 360.00f);
		ImGui::Text("Outer Cutoff:"); ImGui::SameLine(); ImGui::SetNextItemWidth(65.0f);
		ImGui::DragFloat("##OutCut", &m_InOutCutoffDegrees.y, 0.1f, 0.00f, 360.00f);
		ImGui::NewLine();

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

		// --- Tree End ---
		ImGui::TreePop();
	}
}
