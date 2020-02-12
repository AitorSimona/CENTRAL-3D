#include "PanelResources.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "Resources.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"


PanelResources::PanelResources(char* name) : Panel(name)
{
}

PanelResources::~PanelResources()
{
}

bool PanelResources::Draw()
{


	ImGuiWindowFlags resourcesFlags = 0;
	resourcesFlags = ImGuiWindowFlags_NoFocusOnAppearing;
	static ImVec4 instances_color = { 120,120,0,255 };

	if (ImGui::Begin(name, &enabled, resourcesFlags))
	{
		if (ImGui::CollapsingHeader("Folders"))
		{
			for (std::map<uint, ResourceFolder*>::const_iterator it = App->resources->folders.begin(); it != App->resources->folders.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Scenes"))
		{
			for (std::map<uint, ResourceScene*>::const_iterator it = App->resources->scenes.begin(); it != App->resources->scenes.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}

			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Models"))
		{
			for (std::map<uint, ResourceModel*>::const_iterator it = App->resources->models.begin(); it != App->resources->models.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Materials"))
		{
			for (std::map<uint, ResourceMaterial*>::const_iterator it = App->resources->materials.begin(); it != App->resources->materials.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Shaders"))
		{
			for (std::map<uint, ResourceShader*>::const_iterator it = App->resources->shaders.begin(); it != App->resources->shaders.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Meshes"))
		{
			for (std::map<uint, ResourceMesh*>::const_iterator it = App->resources->meshes.begin(); it != App->resources->meshes.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Textures"))
		{
			for (std::map<uint, ResourceTexture*>::const_iterator it = App->resources->textures.begin(); it != App->resources->textures.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Shader Objects"))
		{
			for (std::map<uint, ResourceShaderObject*>::const_iterator it = App->resources->shader_objects.begin(); it != App->resources->shader_objects.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Metas"))
		{
			for (std::map<uint, ResourceMeta*>::const_iterator it = App->resources->metas.begin(); it != App->resources->metas.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}

	}
	ImGui::End();

	return true;
}

void PanelResources::DrawResourceNode(Resource* resource, ImVec4& color)
{
	ImGui::Text(resource->GetName());
	ImGui::SameLine();
	ImGui::Text("Instances: ");
	ImGui::SameLine();
	ImGui::TextColored(color, std::to_string(resource->GetNumInstances()).c_str());
}
