#include "ComponentTransform.h"

#include "GameObject.h"
#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

// MYTODO: Explain math behind transform ops 

ComponentTransform::ComponentTransform(GameObject * ContainerGO) : Component(ContainerGO, Component::ComponentType::Transform)
{
}

ComponentTransform::~ComponentTransform()
{
}

float3 ComponentTransform::GetPosition() const
{
	return position;
}

float3 ComponentTransform::GetScale() const
{
	return scale;
}

float3 ComponentTransform::GetRotation() const
{
	return rotation_euler;
}

float4x4 ComponentTransform::GetLocalTransform() const
{
	return Local_transform;
}

float4x4 ComponentTransform::GetGlobalTransform() const
{
	return Global_transform;
}

float3 ComponentTransform::GetGlobalPosition() const
{
	float4x4 global_transform = GetGlobalTransform();
	return global_transform.TranslatePart();
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	position = float3(x, y, z);

	UpdateLocalTransform();
}

void ComponentTransform::SetRotation(float3 euler_angles)
{
	// --- Compute desired rotation in radians ---
	float3 difference = (euler_angles - rotation_euler) * DEGTORAD;
	Quat quatrot = Quat::FromEulerXYZ(difference.x, difference.y, difference.z);

	// --- Update own variables ---
	rotation = rotation * quatrot;
	rotation_euler = euler_angles;

	// --- Update Transform ---
	UpdateLocalTransform();
}

void ComponentTransform::Scale(float x, float y, float z)
{
	if (x > 0.0f && y > 0.0f && z > 0.0f)
	{
		scale = float3(x, y, z);
		UpdateLocalTransform();
	}
}

void ComponentTransform::SetGlobalTransform(float4x4 new_transform)
{
	float4x4 localTransform = GO->parent->GetComponent<ComponentTransform>()->GetGlobalTransform().Inverted() * new_transform;
	Local_transform = localTransform;
	Global_transform = new_transform;
	update_transform = true;
}

void ComponentTransform::UpdateLocalTransform()
{
	Local_transform = float4x4::FromTRS(position, rotation, scale);
	update_transform = true;
}

void ComponentTransform::OnUpdateTransform(const float4x4 & ParentGlobal)
{
	Global_transform = ParentGlobal * Local_transform;
	UpdateTRS();

	update_transform = false;
}

json ComponentTransform::Save() const
{
	json node;

  	node["positionx"] = std::to_string(position.x);
  	node["positiony"] = std::to_string(position.y);
  	node["positionz"] = std::to_string(position.z);

  	node["rotationx"] = std::to_string(rotation.x);
  	node["rotationy"] = std::to_string(rotation.y);
  	node["rotationz"] = std::to_string(rotation.z);

  	node["scalex"] = std::to_string(scale.x);
  	node["scaley"] = std::to_string(scale.y);
  	node["scalez"] = std::to_string(scale.z);

	return node;
}

void ComponentTransform::Load(json& node)
{
}

void ComponentTransform::CreateInspectorNode()
{
	// --- Transform Position ---
	ImGui::Text("Position  ");
	ImGui::SameLine();

	float3 position = GetPosition();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##PX", &position.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##PY", &position.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##PZ", &position.z, 0.005f);

	// --- Transform Rotation ---
	ImGui::Text("Rotation  ");
	ImGui::SameLine();

	float3 rotation = GetRotation();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##RX", &rotation.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##RY", &rotation.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##RZ", &rotation.z, 0.005f);

	// --- Transform Scale ---
	ImGui::Text("Scale     ");
	ImGui::SameLine();

	float3 scale = GetScale();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SX", &scale.x, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SY", &scale.y, 0.005f);

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	ImGui::DragFloat("##SZ", &scale.z, 0.005f);

	// --- Transform Set ---
	if (!GO->Static)
	{
		if (!GetPosition().Equals(position))
			SetPosition(position.x, position.y, position.z);
		if (!GetScale().Equals(scale))
			Scale(scale.x, scale.y, scale.z);
		if (!GetRotation().Equals(rotation))
			SetRotation(rotation);
	}
}

void ComponentTransform::UpdateTRS()
{
	Local_transform.Decompose(position, rotation, scale);
	rotation_euler = rotation.ToEulerXYZ()*RADTODEG;
}
