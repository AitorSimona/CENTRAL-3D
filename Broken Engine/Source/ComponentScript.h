#ifndef __COMPONENTSCRIPT_H__
#define __COMPONENTSCRIPT_H__

#include "Component.h"
#include "ScriptVar.h"
class ResourceScript;

class ComponentScript : public Component
{
public:
	ComponentScript(GameObject* ContainerGO);
	~ComponentScript();

public:
	void Enable() override;
	void Disable() override;
	void CreateInspectorNode() override;
	void ONResourceEvent(uint UID, Resource::ResourceNotificationType type) override;

	void AssignScript(ResourceScript* script_resource);

	// This function returns the index inside the vector of scriptvars of Script variable using its name as reference
	// Returns -1 if the ScriptVar was not found in the vector
	int ScriptVarAlreadyInComponent(std::string name);

	//Correct SAve & Load from Central 3D
	json Save() const override;
	void Load(json& node) override;
	/*void Save(json &file);
	void Load(json &file);*/

public:
	ResourceScript* script = nullptr;
	std::string script_name;
	std::vector<ScriptVar> script_variables;
};


#endif // !__COMPONENTSCRIPT_H__
