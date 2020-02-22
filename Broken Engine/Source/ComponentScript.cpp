#include "ComponentScript.h"
//#include "ResourceScript.h"
#include "Application.h"
//#include "ModuleResources.h"
#include "ModuleScripting.h"
#include "ModuleFileSystem.h"

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
}

void ComponentScript::AssignScript(std::string relative_path)
{
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
