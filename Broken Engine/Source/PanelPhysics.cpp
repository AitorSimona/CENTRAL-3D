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
	if(!gravity)
		gravity = new PxVec3(App->physics->mScene->getGravity());

	tmpGravity = gravity;

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		ImGui::Text("Gravity:");
		ImGui::Text("X");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PX", &tmpGravity->x, 0.005f);

		ImGui::SameLine();

		ImGui::Text("Y");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PY", &tmpGravity->y, 0.005f);

		ImGui::SameLine();

		ImGui::Text("Z");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(ImGui::GetWindowWidth() * 0.15f);

		ImGui::DragFloat("##PZ", &tmpGravity->z, 0.005f);
	}

	if (tmpGravity != gravity) {
		App->physics->mScene->setGravity(*gravity);
		gravity = tmpGravity;
	}

	ImGui::End();


	return true;
}
