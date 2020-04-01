#include "ScriptingInterface.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ModuleSceneManager.h"
#include "ResourceScene.h"
#include "ScriptData.h"
#include "ComponentProgressBar.h"
#include "ComponentCircularBar.h"
#include "ComponentText.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
#include "GameObject.h"

using namespace Broken;
ScriptingInterface::ScriptingInterface() {}

ScriptingInterface::~ScriptingInterface() {}

void ScriptingInterface::MakeUIComponentVisible(uint go_UUID, const char* comp_type)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go) {

		std::string name = comp_type;
		if (name.compare("Bar") == 0) {
			ComponentProgressBar* comp_bar = go->GetComponent<ComponentProgressBar>();
			comp_bar->visible = true;
		}
		if (name.compare("CiruclarBar") == 0) {
			ComponentCircularBar* comp_bar = go->GetComponent<ComponentCircularBar>();
			comp_bar->visible = true;
		}
		else if (name.compare("Text") == 0) {
			ComponentText* comp_text = go->GetComponent<ComponentText>();
			comp_text->visible = true;
		}
		else if (name.compare("Image") == 0) {
			ComponentImage* comp_image = go->GetComponent<ComponentImage>();
			comp_image->visible = true;
		}
		else if (name.compare("Button") == 0) {
			ComponentButton* comp_button = go->GetComponent<ComponentButton>();
			comp_button->visible = true;
		}
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! GameObject with UUID %d has not a %s component", go_UUID, comp_type);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}

void ScriptingInterface::MakeUIComponentInvisible(uint go_UUID, const char* comp_type)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go) {

		std::string name = comp_type;
		if (name.compare("Bar") == 0) {
			ComponentProgressBar* comp_bar = go->GetComponent<ComponentProgressBar>();
			comp_bar->visible = false;
		}
		if (name.compare("CiruclarBar") == 0) {
			ComponentCircularBar* comp_bar = go->GetComponent<ComponentCircularBar>();
			comp_bar->visible = false;
		}
		else if (name.compare("Text") == 0) {
			ComponentText* comp_text = go->GetComponent<ComponentText>();
			comp_text->visible = false;
		}
		else if (name.compare("Image") == 0) {
			ComponentImage* comp_image = go->GetComponent<ComponentImage>();
			comp_image->visible = false;
		}
		else if (name.compare("Button") == 0) {
			ComponentButton* comp_button = go->GetComponent<ComponentButton>();
			comp_button->visible = false;
		}
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! GameObject with UUID %d has not a %s component", go_UUID, comp_type);
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}

void ScriptingInterface::SetBarPercentage(uint go_UUID, float percentage)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go)
	{
		ComponentProgressBar* bar = go->GetComponent<ComponentProgressBar>();
		if (bar)
			bar->SetPercentage(percentage);
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! ProgressBar component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}

void ScriptingInterface::SetCircularBarPercentage(uint go_UUID, float percentage)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go)
	{
		ComponentCircularBar* bar = go->GetComponent<ComponentCircularBar>();
		if (bar)
			bar->SetPercentage(percentage);
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! CircularProgressBar component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}

void ScriptingInterface::SetUIText(uint go_UUID, const char* text)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go) {
		ComponentText* CompText = go->GetComponent<ComponentText>();

		if (CompText)
			CompText->SetText(text);
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Text Component is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}

void ScriptingInterface::SetUITextAndNumber(uint go_UUID, const char* text, float number)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go) {
		ComponentText* CompText = go->GetComponent<ComponentText>();

		if (CompText && text)
		{
			// String streams aren't very performative, but we need them to keep OK the number's decimals
			std::ostringstream ss;
			ss << number;
			CompText->SetText((text + ss.str()).c_str());
		}
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Text Component or text passed is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}

void ScriptingInterface::SetUITextNumber(uint go_UUID, float number)
{
	GameObject* go = App->scene_manager->currentScene->GetGOWithUID(go_UUID);

	if (go) {
		ComponentText* CompText = go->GetComponent<ComponentText>();

		if (CompText)
		{
			// String streams aren't very performative, but we need them to keep OK the number's decimals
			std::ostringstream ss;
			ss << number;
			CompText->SetText(ss.str().c_str());
		}
		else
			ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Text Component or text passed is NULL");
	}
	else
		ENGINE_CONSOLE_LOG("(SCRIPTING) Alert! Could not find GameObject with UUID %d", go_UUID);
}
