#include "PanelInspector.h"
#include "Imgui/imgui.h"
#include "Application.h"
#include "EngineApplication.h"
#include "ModuleEditorUI.h"
#include "PanelProject.h"
#include "ModulePhysics.h"
#include "ComponentCollider.h"

#include "PhysX_3.4/Include/PxPhysicsAPI.h"

using namespace Broken;
//#include "ModuleSceneManager.h"
//#include "ModuleRenderer3D.h"
//#include "ModuleResourceManager.h"
//#include "ModuleGui.h"

//#include "GameObject.h"
//#include "ComponentTransform.h"
//#include "ComponentMesh.h"
//#include "ComponentMeshRenderer.h"
//#include "ComponentCamera.h"

#include "PanelShaderEditor.h"

//#include "ResourceMesh.h"
//#include "ResourceMaterial.h"
//#include "ResourceTexture.h"
//#include "ResourceShader.h"
//#include "ComponentScript.h"

//#include "mmgr/mmgr.h"

PanelInspector::PanelInspector(char * name) : Panel(name)
{
}

PanelInspector::~PanelInspector()
{
}

bool PanelInspector::Draw()
{
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags settingsFlags = 0;
	settingsFlags = ImGuiWindowFlags_NoFocusOnAppearing;

	if (ImGui::Begin(name, &enabled, settingsFlags))
	{
		Broken::GameObject* Selected = EngineApp->selection->GetLastSelected();
		const std::vector<Broken::GameObject*>* GosSelected = EngineApp->selection->GetSelected();
		Broken::Resource* SelectedRes = EngineApp->editorui->panelProject->GetSelected();

		if (Selected != nullptr)
		{
			// --- Game Object ---
			CreateGameObjectNode(*Selected);

			// --- Components ---

			std::vector<Broken::Component*>* components = &Selected->GetComponents();

			for (std::vector<Broken::Component*>::const_iterator it = components->begin(); it != components->end(); ++it)
			{
				if ((*it) == nullptr)
					continue;

				if (Startup)
					ImGui::SetNextItemOpen(true);

				// SELECTED TODO -> Change script name
				if (*it)
				{
					std::string a = "##Active";
					ImGui::Checkbox((a + (*it)->name).c_str(), &(*it)->GetActive());
					ImGui::SameLine();


					if (ImGui::TreeNodeEx((*it)->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						a = "ComponentOptions";
						ImGui::SameLine();
						if (ImGui::SmallButton("..."))
							ImGui::OpenPopup("Component options");

						if (ImGui::BeginPopup("Component options"))
						{
							bool dummy = false;
							if (ImGui::MenuItem("Delete component"))
							{
								(*it)->to_delete = true;
							}
							if (ImGui::MenuItem("Copy values"))
							{
								EngineApp->selection->CopyComponentValues((*it));
							}
							if (ImGui::MenuItem("Paste values", EngineApp->selection->component_name.c_str(),&dummy,EngineApp->selection->ComponentCanBePasted()))
							{
								EngineApp->selection->PasteComponentValues((*it));
							}
							if (ImGui::MenuItem("Paste values to all selected", EngineApp->selection->component_name.c_str(), &dummy, EngineApp->selection->ComponentCanBePasted()))
							{
								EngineApp->selection->PasteComponentValuesToSelected();
							}
							if (ImGui::BeginMenu("Delete component to all selected"))
							{
								if (ImGui::MenuItem("Confirm delete"))
								{
									EngineApp->selection->DeleteComponentToSelected();

								}
								ImGui::EndMenu();
							}
							ImGui::EndPopup();
						}

						(*it)->CreateInspectorNode();

						ImGui::TreePop();
					}
				}

				ImGui::NewLine();
				ImGui::Separator();
			}

			static ImGuiComboFlags flags = 0;

			const char* items[] = { "Default", "Dynamic RigidBody", "Collider", "Character Controller", "Audio Source", "Particle Emitter", "UI Canvas", "UI Image", "UI Text", "UI Button" };
			static const char* item_current = items[0];

			ImGui::NewLine();

			// --- Add component ---
			if (ImGui::BeginCombo("##Components Combo", "Add Component", flags)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (item_current == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						item_current = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // Set the initial focus when opening the combo (scrolling + for keyboard navigation support in the upcoming navigation branch)
				}
				ImGui::EndCombo();
			}

			// --- Handle drag & drop ---
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("resource"))
				{
					uint UID = *(const uint*)payload->Data;
					Broken::Resource* resource = EngineApp->resources->GetResource(UID, false);

					// MYTODO: Instance resource here, put it on scene (depending on resource)
					if (resource && resource->GetType() == Broken::Resource::ResourceType::SCRIPT)
					{
						resource = EngineApp->resources->GetResource(UID);

						for (Broken::GameObject* obj : *EngineApp->selection->GetSelected())
						{
							Broken::ComponentScript* script = (Broken::ComponentScript*)obj->AddComponent(Broken::Component::ComponentType::Script);
							// SELECTED TODO - It is necessary to assign all the scripts? (move the line below out of the for loop)
							script->AssignScript((Broken::ResourceScript*)resource);
						}

					}
				}

				ImGui::EndDragDropTarget();
			}

			// --- Add here temporal conditions to know which component to add ---

			// MYTODO: Note currently you can not add the same type of component to a go (to be changed)

			Broken::Component::ComponentType type = Broken::Component::ComponentType::Unknown;

			if (item_current == "Mesh")
			{
				type = Broken::Component::ComponentType::Mesh;
			}

			else if (item_current == "Mesh Renderer")
			{
				type = Broken::Component::ComponentType::MeshRenderer;
			}

			else if (item_current == "UI Canvas")
			{
				type = Broken::Component::ComponentType::Canvas;
			}

			else if (item_current == "UI Image")
			{
				type = Broken::Component::ComponentType::Image;
			}

			else if (item_current == "UI Text")
			{
				type = Broken::Component::ComponentType::Text;
			}

			else if (item_current == "UI Button")
			{
				type = Broken::Component::ComponentType::Button;
			}

			else if (item_current == "Dynamic RigidBody")
			{
				type = Broken::Component::ComponentType::DynamicRigidBody;
			}

			else if (item_current == "Collider")
			{
				type = Broken::Component::ComponentType::Collider;
			}
			else if (item_current == "Character Controller")
			{
				type = Broken::Component::ComponentType::CharacterController;
			}
			else if (item_current == "Particle Emitter")
			{
				type = Broken::Component::ComponentType::ParticleEmitter;
			}
			else if (item_current == "Audio Source")
			{
				type = Broken::Component::ComponentType::AudioSource;
			}

			if (type != Broken::Component::ComponentType::Unknown)
			{
				for (Broken::GameObject* obj : *EngineApp->selection->GetSelected())
				{
					obj->AddComponent(type);
				}
			}
			item_current = items[0];

			if (Startup)
				Startup = false;
		}

		// --- Display Resource Information ---
		else if (SelectedRes)
		{
			ImGui::BeginChild("res", ImVec2(0, 35), true);

			ImGui::Text(SelectedRes->GetName());

			ImGui::EndChild();

			ImGui::BeginChild("resdata", ImVec2(ImGui::GetWindowWidth(), ImGui::GetWindowHeight()), false);

			SelectedRes->CreateInspectorNode();

			ImGui::EndChild();
		}
	}

	ImGui::End();

	return true;
}

void PanelInspector::CreateGameObjectNode(Broken::GameObject & Selected) const
{
	ImGui::BeginChild("child", ImVec2(0, 70), true);

	if (ImGui::Checkbox("##GOActive", &Selected.GetActive()))
	{
		if (Selected.GetActive())
			Selected.Enable();
		else
			Selected.Disable();
	}
	ImGui::SameLine();

	// --- Game Object Name Setter ---
	static char GOName[100] = "";
	strcpy_s(GOName, 100, Selected.GetName());
	if (ImGui::InputText("", GOName, 100, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		Selected.SetName(GOName);

	static bool objectStatic = Selected.Static;
	bool checkboxBool = Selected.Static;

	ImGui::SameLine();

	if (ImGui::Checkbox("Static", &checkboxBool)) {
		objectStatic = checkboxBool;
		if (!Selected.childs.empty())
			ImGui::OpenPopup("Static gameObject");
		else
			EngineApp->scene_manager->SetStatic(&Selected, objectStatic,  false);
	}

	ImGui::SetNextWindowSize(ImVec2(400,75));
	if (ImGui::BeginPopup("Static gameObject", ImGuiWindowFlags_NoScrollbar))
	{
		if (Selected.Static) {
			ImGui::Indent(30);
			ImGui::Text("You are about to make this object non-static.");
			ImGui::Spacing();

			ImGui::Unindent(10);
			ImGui::Text("Do you want its children to be non-static aswell?");

			ImGui::Spacing();

			ImGui::Indent(130);
			if (ImGui::Button("Yes")) {
				EngineApp->scene_manager->SetStatic(&Selected, objectStatic, true);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();

			if (ImGui::Button("No")) {
				EngineApp->scene_manager->SetStatic(&Selected, objectStatic, false);
				ImGui::CloseCurrentPopup();
			}
		}
		else {
			ImGui::Indent(30);
			ImGui::Text("You are about to make this object static.");
			ImGui::Spacing();

			ImGui::Unindent(10);
			ImGui::Text("Do you want its children to be static aswell?");

			ImGui::Spacing();

			ImGui::Indent(130);

			if (ImGui::Button("Yes")) {
				EngineApp->scene_manager->SetStatic(&Selected, objectStatic, true);
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();

			if (ImGui::Button("No")) {
				EngineApp->scene_manager->SetStatic(&Selected, objectStatic, false);
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::EndPopup();
	}

	std::vector<Layer>* layers = &App->physics->layer_list;

	static ImGuiComboFlags flags = 0;

	const char* item_current = layers->at(Selected.layer).name.c_str();
	ImGui::Text("Layer: ");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##Layer:", item_current, flags))
	{
		for (int n = 0; n < layers->size(); n++)
		{
			if (!layers->at(n).active)
				continue;

			bool is_selected = (item_current == layers->at(n).name.c_str());
			if (ImGui::Selectable(layers->at(n).name.c_str(), is_selected)) {
				item_current = layers->at(n).name.c_str();
				Selected.layer = layers->at(n).layer;

				ComponentCollider* col = Selected.GetComponent<ComponentCollider>();

				if(col)
					col->UpdateActorLayer(&layers->at(n).layer);
			}
			if (is_selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	else {
		//Selected.layer = layers->at(Selected.layer >> 1).layer;
		//UpdateFilter
	}

	ImGui::EndChild();
}
