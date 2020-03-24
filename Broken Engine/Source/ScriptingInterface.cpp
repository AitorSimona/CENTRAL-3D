#include "ScriptingInterface.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ScriptData.h"
#include "ComponentProgressBar.h"
#include "ComponentText.h"
#include "GameObject.h"

using namespace Broken;
ScriptingInterface::ScriptingInterface() {}

ScriptingInterface::~ScriptingInterface() {}

void ScriptingInterface::MakeUIComponentVisible()
{

}

void ScriptingInterface::ChangeButtonColor()
{

}

void ScriptingInterface::ChangeImageColor()
{

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