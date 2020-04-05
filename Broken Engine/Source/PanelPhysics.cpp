#include "PanelPhysics.h"
#include "Application.h"
#include "EngineApplication.h"
#include "Imgui/imgui.h"
#include "ModulePhysics.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "mmgr/mmgr.h"

using namespace Broken;
PanelPhysics::PanelPhysics(char* name) : Broken::Panel(name)
{
}

PanelPhysics::~PanelPhysics()
{
}

bool PanelPhysics::Draw()
{
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	gravity = EngineApp->physics->mScene->getGravity();
	staticFriction = EngineApp->physics->mMaterial->getStaticFriction();
	dynamicFriction = EngineApp->physics->mMaterial->getDynamicFriction();
	restitution = EngineApp->physics->mMaterial->getRestitution();

	tmpGravity = gravity;
	tmpStaticFriction = staticFriction;
	tmpDynamicFriction = dynamicFriction;
	tmpRestitution = restitution;

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		ImGui::Text("Gravity:");
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PX", &tmpGravity.x, 0.005f);

		ImGui::SameLine();

		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PY", &tmpGravity.y, 0.005f);

		ImGui::SameLine();

		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PZ", &tmpGravity.z, 0.005f);

		ImGui::Text("Material Properties");

		ImGui::Text("Static Friction:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("##staticFriction", &tmpStaticFriction, 0.005f, 0.f, 1.f);

		ImGui::Text("Dynamic Friction:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("##dynamicFriction", &tmpDynamicFriction, 0.005f, 0.f, 1.f);

		ImGui::Text("Restitution:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("##restitution", &tmpRestitution, 0.005f, 0.f, 1.f);

		CreateLayerFilterGrid();

		CreateLayerList();
	}
	ImGui::End();

	if (tmpGravity != gravity) {
		EngineApp->physics->mScene->setGravity(tmpGravity);
	}

	if (tmpStaticFriction != staticFriction) {
		EngineApp->physics->mMaterial->setStaticFriction(tmpStaticFriction);
	}

	if (tmpDynamicFriction != dynamicFriction) {
		EngineApp->physics->mMaterial->setDynamicFriction(tmpDynamicFriction);
	}

	if (tmpRestitution != restitution) {
		EngineApp->physics->mMaterial->setRestitution(tmpRestitution);
	}

	return true;
}

void PanelPhysics::CreateLayerFilterGrid() {

	ImVec2 padding(0.0f, 0.0f);

	ImGui::NewLine();
	ImGui::Text("Layer Collision Matrix");
	ImGui::Separator();
	ImGui::NewLine();

	uint last = 0;
	uint actives = 0;

	for (int i = 0; i < App->physics->layer_list.size(); ++i) { //GET LARGEST TEXT
		Layer* layer = &App->physics->layer_list.at(i);
		int size = ImGui::CalcTextSize(layer->name.c_str()).x;
		if (size > padding.x) padding.x = size, sizeMax = size;
		if (layer->active) last = i, actives++;
	}

	for (int i = 0; i < App->physics->layer_list.size(); ++i) { //HORIZONTAL
		Layer* layer = &App->physics->layer_list.at(i);

		if (!layer->active)
			continue;

		ImGui::Text(layer->name.c_str());

		for (int j = App->physics->layer_list.size() - 1; j - i <= App->physics->layer_list.size(); --j) { //VERTICAL
			Layer* aux_layer = &App->physics->layer_list.at(j);

			if (!aux_layer->active)
				continue;

			std::string st("##" + layer->name + aux_layer->name);

			if(j == last)
				ImGui::SameLine(-ImGui::GetCursorPosX() + padding.x + 22.f);
			else
				ImGui::SameLine();
			
			bool b = layer->active_layers[j];

			if (ImGui::Checkbox(st.c_str(), &b)) {
				layer->active_layers.at(j) = b;
				aux_layer->active_layers.at(i) = layer->active_layers.at(j);
				//UPDATE LAYERS
				layer->UpdateLayerGroup();
				aux_layer->UpdateLayerGroup();
				App->physics->UpdateActorsGroupFilter(&layer->layer);
				App->physics->UpdateActorsGroupFilter(&aux_layer->layer);
			}

		}
	}
	uint count = App->physics->layer_list.size() - 1;
	uint c2 = actives - 1;
	for (int i = count; i >= 0; --i) { //VERTICAL
		Layer layer = App->physics->layer_list.at(i); 
		if (!layer.active)
			continue;

		ImGui::verticalText(layer.name.c_str(), &padding);

		if (i != 0) {
			ImGui::SameLine(26 * (actives - c2--));
			ImGui::Text(" ");
			ImGui::SameLine();
		}
	}

	ImGui::Dummy({ 0,sizeMax });
}

void PanelPhysics::CreateLayerList() {
	ImGui::NewLine();
	ImGui::Text("Layer List");
	ImGui::Separator();
	ImGui::NewLine();

	uint count = App->physics->layer_list.size();
	for (int i = 0; i < count; ++i) {
		std::string name("Layer ");
		name.append(std::to_string(i).c_str());
		name.append(":");
		ImGui::Text(name.c_str());

		char buffer[MAX_TEXT_SIZE] = "";
		if (i < count) {
			ImGui::SameLine();
			strcpy(buffer, App->physics->layer_list.at(i).name.c_str());
			if (ImGui::InputTextWithHint(std::string("##").append(std::to_string(i)).c_str(), "Layer Name", buffer, MAX_TEXT_SIZE, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
			{
				App->physics->layer_list.at(i).name = buffer;
				if (std::strcmp(App->physics->layer_list.at(i).name.c_str(), "")) {
					App->physics->layer_list.at(i).active = true;
				}
				else {
					App->physics->layer_list.at(i).active = false;
				}
			}
		}
	}
}