#include "ComponentCanvas.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleUI.h"
#include "ModuleTextures.h"
#include "PanelScene.h"
#include "ModuleRenderer3D.h"

#include "ComponentText.h"
#include "ComponentImage.h"
#include "ComponentButton.h"
//#include "ComponentCheckBox.h"
//#include "ComponentInputText.h"
#include "ComponentProgressBar.h"
#include "ComponentCircularBar.h"

#include "Imgui/imgui.h"
#include "mmgr/mmgr.h"

using namespace Broken;

ComponentCanvas::ComponentCanvas(GameObject* gameObject) : Component(gameObject, Component::ComponentType::Canvas)
{
	name = "Canvas";
	visible = true;
	App->ui_system->AddCanvas(this);
}

ComponentCanvas::~ComponentCanvas()
{
	App->ui_system->RemoveCanvas(this);
}

void ComponentCanvas::Update()
{
	if (to_delete)
		this->GetContainerGameObject()->RemoveComponent(this);
}

void ComponentCanvas::Draw() const
{
	if (this->active)
	{
		// --- Draw elements inside canvas ---
		for (int i = 0; i < elements.size(); i++)
		{
			if (elements[i]->GetType() == Component::ComponentType::Canvas)
			{
				ComponentCanvas* canvas = (ComponentCanvas*)elements[i];
				if (canvas->visible && canvas->GetActive())
					canvas->Draw();
				continue;
			}
			else if (elements[i]->GetType() == Component::ComponentType::Text)
			{
				ComponentText* text = (ComponentText*)elements[i];
				if (text->visible && text->GetActive())
					text->Draw();
				continue;
			}
			else if (elements[i]->GetType() == Component::ComponentType::Image)
			{
				ComponentImage* image = (ComponentImage*)elements[i];
				if (image->visible && image->GetActive())
					image->Draw();
				continue;
			}
			else if (elements[i]->GetType() == Component::ComponentType::Button)
			{
				ComponentButton* button = (ComponentButton*)elements[i];
				if (button->visible && button->GetActive())
					button->Draw();
			}
			//else if (elements[i]->GetType() == Component::ComponentType::CheckBox)
			//{
			//	CheckBox* elem = (CheckBox*)elements[i];
			//	if (elem->visible) 
			//		elem->Draw();
			//	continue;
			//}
			//else if (elements[i]->GetType() == Component::ComponentType::InputText)
			//{
			//	InputText* elem = (InputText*)elements[i];
			//	if (elem->visible) 
			//		elem->Draw();
			//	continue;
			//}
			else if (elements[i]->GetType() == Component::ComponentType::ProgressBar)
			{
				ComponentProgressBar* bar = (ComponentProgressBar*)elements[i];
				if (bar->visible && bar->GetActive())
					bar->Draw();
				continue;
			}
			else if (elements[i]->GetType() == Component::ComponentType::CircularBar)
			{
				ComponentCircularBar* cbar = (ComponentCircularBar*)elements[i];
				if (cbar->visible && cbar->GetActive())
					cbar->Draw();
				continue;
			}
			else
				continue;
		}
	}
}

json ComponentCanvas::Save() const
{
	json node;
	node["Active"] = this->active;
	node["visible"] = std::to_string(visible);
	node["priority"] = std::to_string(priority);

	return node;
}

void ComponentCanvas::Load(json& node)
{
	this->active = node["Active"].is_null() ? true : (bool)node["Active"];
	std::string visible_str = node["visible"].is_null() ? "0" : node["visible"];
	std::string priority_str = node["priority"].is_null() ? "0" : node["priority"];
	visible = bool(std::stoi(visible_str));
	priority = int(std::stoi(priority_str));
}

void ComponentCanvas::CreateInspectorNode()
{

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
	ImGui::Checkbox("Visible", &visible);
	ImGui::Separator();
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Priority", &priority);
	ImGui::Separator();
}