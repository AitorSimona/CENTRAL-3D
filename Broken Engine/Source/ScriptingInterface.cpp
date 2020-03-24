#include "ScriptingInterface.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ScriptData.h"
#include "ComponentProgressBar.h"
#include "ComponentText.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "GameObject.h"

using namespace Broken;
ScriptingInterface::ScriptingInterface() {}

ScriptingInterface::~ScriptingInterface() {}

void ScriptingInterface::MakeUIComponentVisible(uint component_UUID, const char* comp_type)
{
	Component* comp = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponentWithUID(component_UUID);

	if (comp) {
		
		std::string name = comp_type;
		if (name.compare("Bar") == 0) {
			ComponentProgressBar* comp_bar = (ComponentProgressBar*)comp;
			comp_bar->visible = true;
		}
		else if (name.compare("Text") == 0) {
			ComponentText* comp_text = (ComponentText*)comp;
			comp_text->visible = true;
		}
		else if (name.compare("Image") == 0) {
			ComponentImage* comp_image = (ComponentImage*)comp;
			comp_image->visible = true;
		}
		else if (name.compare("Button") == 0) {
			ComponentButton* comp_button = (ComponentButton*)comp;
			comp_button->visible = true;
		}
		else
			ENGINE_CONSOLE_LOG("Component with UUID %d is not a %s component", component_UUID, comp_type);
	}
	else
		ENGINE_CONSOLE_LOG("Could not find %s component inside %s GameObject", comp_type, App->scripting->current_script->my_component->GetContainerGameObject()->GetName());
}

void ScriptingInterface::MakeUIComponentInvisible(uint component_UUID, const char* comp_type)
{
	Component* comp = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponentWithUID(component_UUID);

	if (comp) {

		std::string name = comp_type;
		if (name.compare("Bar") == 0) {
			ComponentProgressBar* comp_bar = (ComponentProgressBar*)comp;
			comp_bar->visible = false;
		}
		else if (name.compare("Text") == 0) {
			ComponentText* comp_text = (ComponentText*)comp;
			comp_text->visible = false;
		}
		else if (name.compare("Image") == 0) {
			ComponentImage* comp_image = (ComponentImage*)comp;
			comp_image->visible = false;
		}
		else if (name.compare("Button") == 0) {
			ComponentButton* comp_button = (ComponentButton*)comp;
			comp_button->visible = false;
		}
		else
			ENGINE_CONSOLE_LOG("Component with UUID %d is not a %s component", component_UUID, comp_type);
	}
	else
		ENGINE_CONSOLE_LOG("Could not found %s component inside %s GameObject", comp_type, App->scripting->current_script->my_component->GetContainerGameObject()->GetName());
}

void ScriptingInterface::SetBarPercentage(float percentage)
{
	ComponentProgressBar* bar = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentProgressBar>();

	if (bar)
		bar->SetPercentage(percentage);
	else
		ENGINE_CONSOLE_LOG("[Script]: ProgressBar component is NULL");
}

void ScriptingInterface::SetUIText(const char* text)
{
	ComponentText* CompText = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentText>();

	if (CompText)
		CompText->SetText(text);
	else
		ENGINE_CONSOLE_LOG("[Script]: Text Component is NULL");
}

void ScriptingInterface::SetUITextAndNumber(const char* text, float number)
{
	ComponentText* CompText = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentText>();

	if (CompText && text)
	{
		//String streams aren't very performative, but we need them to keep OK the number's decimals
		std::ostringstream ss;
		ss << number;
		CompText->SetText((text + ss.str()).c_str());
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Text Component or text passed is NULL");
}

void ScriptingInterface::SetUITextNumber(float number)
{
	ComponentText* CompText = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentText>();

	if (CompText)
	{
		//String streams aren't very performative, but we need them to keep OK the number's decimals
		std::ostringstream ss;
		ss << number;
		CompText->SetText(ss.str().c_str());
	}
	else
		ENGINE_CONSOLE_LOG("[Script]: Text Component or text passed is NULL");
}