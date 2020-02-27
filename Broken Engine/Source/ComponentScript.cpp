#include "ComponentScript.h"
//#include "ResourceScript.h"
#include "Application.h"
//#include "ModuleResources.h"
#include "ModuleScripting.h"
#include "ModuleFileSystem.h"
#include "ResourceScript.h"
#include "Imgui/imgui.h"
#include "ModuleGui.h"

ComponentScript::ComponentScript(GameObject* ContainerGO) : Component(ContainerGO, Component::ComponentType::Script)
{
	type = ComponentType::Script;
}

ComponentScript::~ComponentScript() 
{
	App->scripting->DeleteScriptInstanceWithParentComponent(this);
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

	std::string name = script->script_name + "(Script)";
	ImGui::Checkbox("Active", &active); ImGui::SameLine();
	if (ImGui::TreeNodeEx(name.data(), base_flags)) {
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
				float auxVal(script_variables[i].editor_value.as_double_number);

				if (ImGui::DragFloat(auxName.c_str(), &auxVal, 0.05f)) {
					script_variables[i].editor_value.as_double_number = auxVal;
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

		ImGui::TreePop();
	}
}

//Assigns the resource to the component and sends the script to the module so it can be compiled & used
void ComponentScript::AssignScript(ResourceScript* script_resource)
{
	if (script_resource != nullptr)
		this->script = script_resource;

	script_name = this->script->script_name;

	//Send Component info to scripting to create a Script Instance / Lua class
	App->scripting->SendScriptToModule(this);
	//MYTODO: Dídac trying to compile
	/*ResourceScript* new_script = (ResourceScript*)App->resources->CreateNewResource(Resource::SCRIPT);
	this->script = new_script;
	std::string filename;
	App->file_system->SplitFilePath(relative_path.data(),nullptr,&filename,nullptr);
	this->script_name = filename;
	this->script->script_name = filename;
	this->script->relative_path = relative_path;

	std::string absolute_path = App->file_system->GetPathToGameFolder(true) + relative_path;
	this->script->absolute_path = absolute_path;
	App->scripting->SendScriptToModule(this, absolute_path);*/
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
	return json();
}

void ComponentScript::Load(json& node)
{


}

//void ComponentScript::Save(json & file)
//{
//	file["UID"] = this->UID;
//	file["Active"] = this->active;
//	file["Script_name"] = this->script_name.c_str();
//	if (this->script != nullptr)
//	{
//		file["Resource_UID"] = this->script->GetUID();
//		file["Script_Path"] = this->script->relative_path;
//	}
//}
//
//void ComponentScript::Load(json & file)
//{
//	this->UID = file["UID"];
//	uint32 uid = file["Resource_UID"];
//	this->active = file["Active"];
//	std::string name_of_script = file["Script_name"];
//	std::string path_of_script = file["Script_Path"];
//
//	this->script_name = name_of_script;
//
//	script = (ResourceScript*)App->resources->Get(uid);
//	if (script != nullptr)
//	{
//		script->AddReference();
//	}
//	else
//	{
//		script = (ResourceScript*)App->resources->CreateNewResource(Resource::SCRIPT, uid);
//	}
//	script->script_name = name_of_script;
//	script->relative_path = path_of_script;
//	script->absolute_path = App->file_system->GetPathToGameFolder(true) + path_of_script;
//
//	App->scripting->SendScriptToModule(this,path_of_script);
//}
