#include "ComponentScript.h"
//#include "ResourceScript.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleScripting.h"
#include "ModuleFileSystem.h"
#include "ResourceScript.h"
#include "Imgui/imgui.h"
#include "ModuleGui.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"

ComponentScript::ComponentScript(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Script)
{
	type = ComponentType::Script;
}

ComponentScript::~ComponentScript()
{
	App->scripting->DeleteScriptInstanceWithParentComponent(this);

	if (script && script->IsInMemory())
	{
		script->Release();
		script->RemoveUser(GO);
	}
}

void ComponentScript::Enable()
{
	active = true;
}

void ComponentScript::Disable()
{
	active = false;
}

void ComponentScript::CreateInspectorNode()
{
	ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanAvailWidth;

	std::string name = this->script_name + "(Script)";
	ImGui::Checkbox("Active", &active); ImGui::SameLine();

	if (ImGui::TreeNodeEx(name.data(), base_flags))
	{
		if (this->script != nullptr)
		{
			if (ImGui::Button("Open Script File")) {
				App->gui->RequestBrowser(std::string(script->absolute_path).data());
			}

			char auxBuffer[256];

			//Display Variables
			for (int i = 0; i < script_variables.size(); ++i)
			{
				std::string auxName = script_variables[i].name.c_str();
				ImGui::Text(auxName.c_str()); ImGui::SameLine(200.f); ImGui::SetNextItemWidth(ImGui::GetWindowWidth() / 7.0f);
				auxName.assign("##Var" + auxName);

				VarType type = script_variables[i].type;
				if (type == VarType::DOUBLE)
				{
					float auxVal(script_variables[i].editor_value.as_double);

					if (ImGui::DragFloat(auxName.c_str(), &auxVal, 0.05f)) {
						script_variables[i].editor_value.as_double = auxVal;
						script_variables[i].changed_value = true;
					}

				}
				else if (type == VarType::BOOLEAN)
				{
					if (ImGui::Checkbox(auxName.c_str(), &script_variables[i].editor_value.as_boolean))
						script_variables[i].changed_value = true;
				}
				else if (type == VarType::STRING)
				{
					strcpy(auxBuffer, script_variables[i].editor_value.as_string);

					ImGui::InputText(auxName.c_str(), auxBuffer, IM_ARRAYSIZE(auxBuffer));

					if (strcmp(script_variables[i].editor_value.as_string, auxBuffer) != 0) {
						strcpy(script_variables[i].editor_value.as_string, auxBuffer);
						script_variables[i].changed_value = true;
					}
				}
			}
		}

		ImGui::TreePop();
	}
}

void ComponentScript::ONResourceEvent(uint UID, Resource::ResourceNotificationType type)
{
	// --- Always check if your resources are already invalidated, since go sends events from all of its components resources ---
	switch (type)
	{
	case Resource::ResourceNotificationType::Overwrite:
		if(script && UID == script->GetUID())

		break;

	case Resource::ResourceNotificationType::Deletion:
		if (script && UID == script->GetUID())
		{
			script = nullptr;
			App->scripting->NullifyScriptInstanceWithParentComponent(this);
		}
		break;

	default:
		break;
	}
}

// Assigns the resource to the component and sends the script to the module so it can be compiled & used
void ComponentScript::AssignScript(ResourceScript* script_resource)
{
	if (script_resource != nullptr)
	{
		this->script = script_resource;
		script_resource->AddUser(GO);
	}

	script_name = this->script->script_name;

	// Send Component info to scripting to create a Script Instance / Lua class
	App->scripting->SendScriptToModule(this);
}

int ComponentScript::ScriptVarAlreadyInComponent(std::string name)
{
	int ret = -1;

	for (int i = 0; i < script_variables.size(); ++i)
	{
		if (!name.compare(script_variables[i].name))
		{
			ret = i;
			break;
		}
	}
	return ret;
}

json ComponentScript::Save() const
{
	json node;
	node["Resources"]["ResourceScript"];

	// Store path to component file
	if(script)
		node["Resources"]["ResourceScript"] = std::string(script->GetResourceFile());

	// Save the public variables of the script
	char name[50];
	node["Script variables"]["Count"] = script_variables.size();

	for (int i = 0; i < script_variables.size(); ++i) {

		sprintf_s(name, 50, "Variable %d", i);
		node["Script variables"][name]["Name"] = script_variables[i].name.c_str();

		switch (script_variables[i].type) 
		{
		case VarType::BOOLEAN:
			node["Script variables"][name]["Type"] = "Boolean";
			node["Script variables"][name]["Value"] = script_variables[i].editor_value.as_boolean;
			break;

		case VarType::DOUBLE:
			node["Script variables"][name]["Type"] = "Double";
			node["Script variables"][name]["Value"] = script_variables[i].editor_value.as_double;
			break;

		case VarType::STRING:
			node["Script variables"][name]["Type"] = "String";
			node["Script variables"][name]["Value"] = script_variables[i].editor_value.as_string;
			break;
		}
	}

	return node;
}

void ComponentScript::Load(json& node)
{
	std::string path = node["Resources"]["ResourceScript"];

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

	if (IMeta)
	{
		ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path.c_str());

		if (meta)
			script = (ResourceScript*)App->resources->GetResource(meta->GetUID());

		// --- We want to be notified of any resource event ---
		if (script)
			script->AddUser(GO);

		AssignScript((ResourceScript*)this->script);
	}

	// Load the public variables of the script
	char name[50];
	uint cnt = node["Script variables"]["Count"];

	for (int i = 0; i < cnt; ++i) {

		sprintf_s(name, 50, "Variable %d", i);
		json js1 = node["Script variables"][name]["Name"];
		script_variables[i].name = js1.get<std::string>(); 

		json js2 = node["Script variables"][name]["Type"];
		std::string type = js2.get<std::string>();

		if (type.compare("Boolean") == 0) {
			script_variables[i].type = VarType::BOOLEAN;
			script_variables[i].editor_value.as_boolean = node["Script variables"][name]["Value"];
		}
		else if (type.compare("Double") == 0) {
			script_variables[i].type = VarType::DOUBLE;
			script_variables[i].editor_value.as_double = node["Script variables"][name]["Value"];
		} 
		else if (type.compare("String") == 0) {
			script_variables[i].type = VarType::STRING;
			json js3 = node["Script variables"][name]["Value"];
			std::string as_string = js3.get<std::string>();
			script_variables[i].ChangeEditorValue(as_string.c_str());
		}
	}
}