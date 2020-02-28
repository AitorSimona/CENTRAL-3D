#include "PanelPhysics.h"
#include "Application.h"
#include "ModuleGui.h"
#include "Imgui/imgui.h"
#include "ModulePhysics.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

#include "mmgr/mmgr.h"

PanelPhysics::PanelPhysics(char* name) : Panel(name)
{
}

PanelPhysics::~PanelPhysics()
{
}

bool PanelPhysics::Draw()
{
	gravity = App->physics->mScene->getGravity();
	staticFriction = App->physics->mMaterial->getStaticFriction();
	dynamicFriction = App->physics->mMaterial->getDynamicFriction();
	restitution = App->physics->mMaterial->getRestitution();

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

		ImGui::Text("Static Friction:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("##dynamicFriction", &tmpDynamicFriction, 0.005f, 0.f, 1.f);

		ImGui::Text("Restitution:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);
		ImGui::DragFloat("##restitution", &tmpRestitution, 0.005f, 0.f, 1.f);

		//CreateLayerFilterGrid();
	}

	if (tmpGravity != gravity) {
		App->physics->mScene->setGravity(tmpGravity);
	}

	if (tmpStaticFriction != staticFriction) {
		App->physics->mMaterial->setStaticFriction(tmpStaticFriction);
	}

	if (tmpDynamicFriction != dynamicFriction) {
		App->physics->mMaterial->setDynamicFriction(tmpDynamicFriction);
	}

	if (tmpRestitution != restitution) {
		App->physics->mMaterial->setRestitution(tmpRestitution);
	}

	ImGui::End();


	return true;
}
