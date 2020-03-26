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

	for (int i = 0; i < App->physics->layer_list.size(); ++i) { //GET LARGEST TEXT
		Layer* layer = &App->physics->layer_list.at(i);
		float size = ImGui::CalcTextSize(layer->name.c_str()).x;
		if (size > padding.x) padding.x = size;
	}

	for (int i = 0; i < App->physics->layer_list.size(); ++i) { //HORIZONTAL
		Layer* layer = &App->physics->layer_list.at(i);
		ImGui::Text(layer->name.c_str());

		for (int j = App->physics->layer_list.size() - 1; j - i <= App->physics->layer_list.size(); --j) { //VERTICAL
			Layer* aux_layer = &App->physics->layer_list.at(j);
			std::string st("##" + layer->name + aux_layer->name);

			if(j == App->physics->layer_list.size() - 1)
				ImGui::SameLine(-ImGui::GetCursorPosX() + padding.x + 20.f);
			else
				ImGui::SameLine();
			
			bool b = layer->active_layers[j];

			if (ImGui::Checkbox(st.c_str(), &b)) {
				layer->active_layers.at(j) = b;
				aux_layer->active_layers.at(i) = layer->active_layers.at(j);
				//UPDATE LAYERS
				layer->UpdateLayerGroup();
				aux_layer->UpdateLayerGroup();
				App->physics->UpdateActors(&layer->layer);
				App->physics->UpdateActors(&aux_layer->layer);
			}

		}
	}

	for (int i = App->physics->layer_list.size() - 1; i >= 0; --i) { //VERTICAL
		Layer layer = App->physics->layer_list.at(i);
		ImGui::verticalText(layer.name.c_str(), &padding);

		if (i != 0) {
			ImGui::SameLine(25 * (App->physics->layer_list.size() - i));
			ImGui::Text(" ");
			ImGui::SameLine();
		}
	}
}