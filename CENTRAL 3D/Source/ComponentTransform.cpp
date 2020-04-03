#include "ComponentTransform.h"

#include "GameObject.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

// MYTODO: Explain math behind transform ops 

ComponentTransform::ComponentTransform(GameObject * ContainerGO) : Component(ContainerGO, Component::ComponentType::Transform)
{
	name = "Transform";
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

void ComponentTransform::SetQuatRotation(Quat rot)
{
	rotation = rot;
	rotation_euler = rotation.ToEulerXYZ();
	rotation_euler *= RADTODEG;

	// --- Update Transform ---
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

  	node["positionx"] = position.x;
  	node["positiony"] = position.y;
  	node["positionz"] = position.z;

  	node["rotationx"] = rotation.x;
  	node["rotationy"] = rotation.y;
  	node["rotationz"] = rotation.z;
	node["rotationw"] = rotation.w;

  	node["scalex"] = scale.x;
  	node["scaley"] = scale.y;
  	node["scalez"] = scale.z;

	return node;
}

void ComponentTransform::Load(json& node)
{
	float posx = node["positionx"].is_null() ? "0" : node["positionx"];
	float posy = node["positiony"].is_null() ? "0" : node["positiony"];
	float posz = node["positionz"].is_null() ? "0" : node["positionz"];
	
	float rotx = node["rotationx"].is_null() ? "0" : node["rotationx"];
	float roty = node["rotationy"].is_null() ? "0" : node["rotationy"];
	float rotz = node["rotationz"].is_null() ? "0" : node["rotationz"];
	float rotw = node["rotationw"].is_null() ? "0" : node["rotationw"];
	
	float scalex = node["scalex"].is_null() ? "0" : node["scalex"];
	float scaley = node["scaley"].is_null() ? "0" : node["scaley"];
	float scalez = node["scalez"].is_null() ? "0" : node["scalez"];

	SetPosition(posx, posy, posz);
	SetQuatRotation(Quat(rotx, roty, rotz, rotw));
	Scale(scalex, scaley, scalez);
}

void ComponentTransform::UpdateTRS()
{
	Local_transform.Decompose(position, rotation, scale);
	rotation_euler = rotation.ToEulerXYZ()*RADTODEG;
}


void ComponentTransform::CreateInspectorNode()
{
	// --- Transform Position ---
	ImGui::Text("Position  ");
	ImGui::SameLine();

	float dragSpeed = 0.25f;

	float3 position = GetPosition();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##PX", &position.x, dragSpeed));

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##PY", &position.y, dragSpeed));

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##PZ", &position.z, dragSpeed));

	// --- Transform Rotation ---
	ImGui::Text("Rotation  ");
	ImGui::SameLine();

	float3 rotation = GetRotation();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##RX", &rotation.x, dragSpeed));

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##RY", &rotation.y, dragSpeed));

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##RZ", &rotation.z, dragSpeed));

	// --- Transform Scale ---
	float scale_dragSpeed = 0.1f;

	ImGui::Text("Scale     ");
	ImGui::SameLine();

	float3 scale = GetScale();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##SX", &scale.x, dragSpeed));

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##SY", &scale.y, dragSpeed));

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
	ImGui::SameLine();

	if (ImGui::DragFloat("##SZ", &scale.z, dragSpeed));

	// --- Transform Set ---
	if (!GO->Static) {
		if (!GetPosition().Equals(position))
			SetPosition(position.x, position.y, position.z);
		if (!GetScale().Equals(scale))
			Scale(scale.x, scale.y, scale.z);
		if (!GetRotation().Equals(rotation))
			SetRotation(rotation);
	}
}

