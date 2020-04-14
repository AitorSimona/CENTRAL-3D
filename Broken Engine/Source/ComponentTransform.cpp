#include "ComponentTransform.h"
#include "Application.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "Math.h"
#include "ModuleSceneManager.h"

#include "mmgr/mmgr.h"

// MYTODO: Explain math behind transform ops 
using namespace Broken;

ComponentTransform::ComponentTransform(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Transform) {
	name = "Transform";
}

ComponentTransform::~ComponentTransform() {
}

float3 ComponentTransform::GetPosition() const {
	return position;
}

float3 ComponentTransform::GetScale() const {
	return scale;
}

float3 ComponentTransform::GetRotation() const {
	return rotation_euler;
}

Quat ComponentTransform::GetQuaternionRotation() const
{
	return rotation;
}

float4x4 ComponentTransform::GetLocalTransform() const
{
	return Local_transform;
}

float4x4 ComponentTransform::GetGlobalTransform() const {
	return Global_transform;
}

float3 ComponentTransform::GetGlobalPosition() const {
	float4x4 global_transform = GetGlobalTransform();
	return global_transform.TranslatePart();
}

void ComponentTransform::SetPosition(float3 new_position)
{
	position = new_position;
	UpdateLocalTransform();
}

void ComponentTransform::SetPosition(float x, float y, float z)
{
	position = float3(x, y, z);
	UpdateLocalTransform();
}

void ComponentTransform::SetRotation(float3 euler_angles) {
	// --- Compute desired rotation in radians ---
	float3 difference = (euler_angles - rotation_euler) * DEGTORAD;
	Quat quatrot = Quat::FromEulerXYZ(difference.x, difference.y, difference.z);

	// --- Update own variables ---
	rotation = rotation * quatrot;
	rotation_euler = euler_angles;

	// --- Update Transform ---
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

void ComponentTransform::SetRotation(Quat quat)
{
	// --- Update own variables ---
	rotation = quat;
	rotation_euler = quat.ToEulerXYZ() * RADTODEG;

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

void ComponentTransform::SetGlobalTransform(float4x4 new_transform) {
	float4x4 localTransform = GO->parent->GetComponent<ComponentTransform>()->GetGlobalTransform().Inverted() * new_transform;
	Local_transform = localTransform;
	Global_transform = new_transform;
	update_transform = true;
}

void ComponentTransform::UpdateLocalTransform() {
	Local_transform = float4x4::FromTRS(position, rotation, scale);
	update_transform = true;
}

void ComponentTransform::OnUpdateTransform(const float4x4& ParentGlobal) {

	Global_transform = ParentGlobal * Local_transform;
	UpdateTRS();
	GO->UpdateAABB();
	if (GO->Static)
		App->scene_manager->update_tree = true;
	update_transform = false;
}

json ComponentTransform::Save() const {
	json node;

	node["layer"] = std::to_string(GO->layer);

	node["positionx"] = std::to_string(position.x);
	node["positiony"] = std::to_string(position.y);
	node["positionz"] = std::to_string(position.z);

  	node["rotationx"] = std::to_string(rotation.x);
  	node["rotationy"] = std::to_string(rotation.y);
  	node["rotationz"] = std::to_string(rotation.z);
	node["rotationw"] = std::to_string(rotation.w);

	node["scalex"] = std::to_string(scale.x);
	node["scaley"] = std::to_string(scale.y);
	node["scalez"] = std::to_string(scale.z);

	return node;
}

void ComponentTransform::Load(json& node)
{
	std::string layer = node["layer"].is_null() ? "0" : node["layer"];
	std::string posx = node["positionx"].is_null() ? "0" : node["positionx"];
	std::string posy = node["positiony"].is_null() ? "0" : node["positiony"];
	std::string posz = node["positionz"].is_null() ? "0" : node["positionz"];

	std::string rotx = node["rotationx"].is_null() ? "0" : node["rotationx"];
	std::string roty = node["rotationy"].is_null() ? "0" : node["rotationy"];
	std::string rotz = node["rotationz"].is_null() ? "0" : node["rotationz"];
	std::string rotw = node["rotationw"].is_null() ? "0" : node["rotationw"];

	std::string scalex = node["scalex"].is_null() ? "0" : node["scalex"];
	std::string scaley = node["scaley"].is_null() ? "0" : node["scaley"];
	std::string scalez = node["scalez"].is_null() ? "0" : node["scalez"];

	GO->layer = std::stof(layer);

	float3 pos = float3(std::stof(posx), std::stof(posy), std::stof(posz));

	SetPosition(pos);
	SetQuatRotation(Quat(std::stof(rotx), std::stof(roty), std::stof(rotz), std::stof(rotw)));
	Scale(std::stof(scalex), std::stof(scaley), std::stof(scalez));
}

void ComponentTransform::CreateInspectorNode()
{
	updateValues = false;

	// --- Transform Position ---
	ImGui::Text("Position  ");
	ImGui::SameLine();

	float dragSpeed = 0.25f;

	float3 position = GetPosition();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##PX", &position.x, dragSpeed)) updateValues = true;

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##PY", &position.y, dragSpeed)) updateValues = true;

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##PZ", &position.z, dragSpeed)) updateValues = true;

	// --- Transform Rotation ---
	ImGui::Text("Rotation  ");
	ImGui::SameLine();

	float3 rotation = GetRotation();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##RX", &rotation.x, dragSpeed)) updateValues = true;

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##RY", &rotation.y, dragSpeed)) updateValues = true;

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##RZ", &rotation.z, dragSpeed)) updateValues = true;

	// --- Transform Scale ---
	float scale_dragSpeed = 0.1f;

	ImGui::Text("Scale     ");
	ImGui::SameLine();

	float3 scale = GetScale();
	ImGui::Text("X");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##SX", &scale.x, scale_dragSpeed)) updateValues = true;

	ImGui::SameLine();

	ImGui::Text("Y");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##SY", &scale.y, scale_dragSpeed)) updateValues = true;

	ImGui::SameLine();

	ImGui::Text("Z");
	ImGui::SameLine();
	ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

	if (ImGui::DragFloat("##SZ", &scale.z, scale_dragSpeed)) updateValues = true;

	// --- Transform Set ---
	if (!GO->Static) {
		if (!GetPosition().Equals(position))
			SetPosition(position);
		if (!GetScale().Equals(scale))
			Scale(scale.x, scale.y, scale.z);
		if (!GetRotation().Equals(rotation))
			SetRotation(rotation);
	}
}

void ComponentTransform::UpdateTRS() {
	Local_transform.Decompose(position, rotation, scale);
	rotation_euler = rotation.ToEulerXYZ() * RADTODEG;
}
