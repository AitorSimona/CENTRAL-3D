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
		// SELECTED TODO
		// Displaying the minimum common inspector of the selection
		//Broken::GameObject* Selected = EngineApp->selection->GetSelected()->size() <= 1 ? EngineApp->selection->GetLastSelected() : EngineApp->selection->root;

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

				if (*it)
				{
					std::string name = "##Active";
					if (ImGui::Checkbox((name + (*it)->name).c_str(), &(*it)->GetActive()))
					{
						if ((*it)->IsEnabled())
							(*it)->Enable();
						else
							(*it)->Disable();
					}
					ImGui::SameLine();

					name = (*it)->GetType() != Component::ComponentType::Script ? (*it)->name : (*it)->name + " (Script)";

					if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
					{
						CreateComponentOptionsMenu(it);

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

void PanelInspector::CreateComponentOptionsMenu(std::vector<Broken::Component*>::const_iterator& it)
{
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
		if (ImGui::MenuItem("Paste values", EngineApp->selection->component_name.c_str(), &dummy, EngineApp->selection->ComponentCanBePasted()))
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
}

// SELECTED TODO: test editing for multiselection
// OPTIMIZE DEFAULT SHOWN PROPERTIES -> LESS SELECTION ITERATIONS
void PanelInspector::CreateGameObjectNode(Broken::GameObject & Selected) const
{
	ImGui::BeginChild("child", ImVec2(0, 70), true);

	// Changing active state to true if all are selected and to false if at least one is not active
	bool active = true;
	for (GameObject* obj : *App->selection->GetSelected())
	{
		if (obj->GetActive() == false) {
			active = false;
			break;
		}
	}

	if (ImGui::Checkbox("##GOActive", &active))
	{
		if (active)
			for (GameObject* obj : *App->selection->GetSelected())
				obj->Enable();
		else
			for (GameObject* obj : *App->selection->GetSelected())
				obj->Disable();
	}
	ImGui::SameLine();

	// --- Game Object Name Setter ---
	static char GOName[128] = "";
	static std::string number = "";
	strcpy_s(GOName, 128, Selected.GetName());

	if (ImGui::InputText("", GOName, 128, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll))
		for (int i = 0; i < App->selection->GetSelected()->size(); i++)
		{
			number = " (" + std::to_string(i) + ")";
			number = GOName + number;
			//std::strcat(GOName, number.c_str());

			App->selection->GetSelected()->at(i)->SetName(number.c_str());
		}


	bool objectStatic = true;
	bool checkbox_static = true;
	bool exists_childs = false;
	for (int i=0; i < App->selection->GetSelected()->size() && (checkbox_static || !exists_childs);i++)
	{
		if (App->selection->GetSelected()->at(i)->Static == false) 
			checkbox_static = false;

		if (App->selection->GetSelected()->at(i)->childs.empty() == false)
			exists_childs = true;
	}

	ImGui::SameLine();

	if (ImGui::Checkbox("Static", &checkbox_static)) {
		objectStatic = checkbox_static;
		if (exists_childs)
			ImGui::OpenPopup("Static gameObject");
		else
			for (GameObject* obj : *App->selection->GetSelected())
				EngineApp->scene_manager->SetStatic(obj, objectStatic,  false);
	}

	ImGui::SetNextWindowSize(ImVec2(400,75));
	if (ImGui::BeginPopup("Static gameObject", ImGuiWindowFlags_NoScrollbar))
	{
		static std::string text = (!objectStatic) ? "You are about to make objects non-static.\nDo you want to edit the children aswell?" :
			"You are about to make this objects static.\nDo you want to edit the children aswell?";

		ImGui::Text(text.c_str());

		ImGui::Indent(130);
		if (ImGui::Button("Yes")) {
			for (GameObject* obj : *App->selection->GetSelected())
				EngineApp->scene_manager->SetStatic(obj, objectStatic, true);
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();

		if (ImGui::Button("No")) {
			for (GameObject* obj : *App->selection->GetSelected())
				EngineApp->scene_manager->SetStatic(obj, objectStatic, false);
			ImGui::CloseCurrentPopup();
		}

		/*ImGui::Indent(30);
		ImGui::Text("You are about to make this object non-static.");
		ImGui::Spacing();

		ImGui::Unindent(10);
		ImGui::Text("Do you want its children to be non-static aswell?");

		ImGui::Spacing();*/




		ImGui::EndPopup();
	}

	std::vector<Layer>* layers = &App->physics->layer_list;

	static ImGuiComboFlags flags = 0;

	//int layer = App->selection->GetLastSelected()->GetLayer();
	int layer = Selected.GetLayer();

	std::string layer_name = layers->at(layer).name.c_str();
	for (GameObject* obj : *App->selection->GetSelected())
	{
		if (layer != obj->GetLayer())
		{
			layer_name = "---";
			layer = -1;
			break;
		}
	}

	//const char* item_current = layers->at(Selected.layer).name.c_str();
	ImGui::Text("Layer: ");
	ImGui::SameLine();
	if (ImGui::BeginCombo("##Layer:", layer_name.c_str(), flags))
	{
		for (int n = 0; n < layers->size(); n++)
		{
			if (!layers->at(n).active)
				continue;

			bool is_selected = (layer_name == layers->at(n).name.c_str());

			if (ImGui::Selectable(layers->at(n).name.c_str(), is_selected)) {
				// Changing layer
				for (GameObject* obj : *App->selection->GetSelected())
				{
					layer_name = layers->at(n).name.c_str();
					obj->layer = layers->at(n).layer;

					ComponentCollider* col = obj->GetComponent<ComponentCollider>();

					if(col)
						col->UpdateActorLayer(&layers->at(n).layer);
				}
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
