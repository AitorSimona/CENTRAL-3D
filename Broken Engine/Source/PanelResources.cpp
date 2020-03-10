#include "PanelResources.h"
#include "EngineApplication.h"
#include "Resources.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"


PanelResources::PanelResources(char* name) : Broken::Panel(name)
{
}

PanelResources::~PanelResources()
{
}

bool PanelResources::Draw()
{
	// --- Simply print on screen all resources references number (debug purposes) ---
	ImGui::SetCurrentContext(EngineApp->gui->getImgUICtx());

	ImGuiWindowFlags resourcesFlags = 0;
	resourcesFlags = ImGuiWindowFlags_NoFocusOnAppearing;
	static ImVec4 instances_color = { 120,120,0,255 };

	if (ImGui::Begin(name, &enabled, resourcesFlags))
	{
		if (ImGui::CollapsingHeader("Folders"))
		{
			for (std::map<uint, Broken::ResourceFolder*>::const_iterator it = EngineApp->resources->folders.begin(); it != EngineApp->resources->folders.end(); ++it)
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
			for (std::map<uint, Broken::ResourceScene*>::const_iterator it = EngineApp->resources->scenes.begin(); it != EngineApp->resources->scenes.end(); ++it)
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
			for (std::map<uint, Broken::ResourceModel*>::const_iterator it = EngineApp->resources->models.begin(); it != EngineApp->resources->models.end(); ++it)
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
			for (std::map<uint, Broken::ResourceMaterial*>::const_iterator it = EngineApp->resources->materials.begin(); it != EngineApp->resources->materials.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Animations"))
		{
			for (std::map<uint, Broken::ResourceAnimation*>::const_iterator it = EngineApp->resources->animations.begin(); it != EngineApp->resources->animations.end(); ++it)
			{
				if ((*it).second)
				{
					DrawResourceNode((*it).second, instances_color);
				}
			}

			ImGui::Separator();
		}
		if (ImGui::CollapsingHeader("Bones"))
		{
			for (std::map<uint, Broken::ResourceBone*>::const_iterator it = EngineApp->resources->bones.begin(); it != EngineApp->resources->bones.end(); ++it)
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
			for (std::map<uint, Broken::ResourceShader*>::const_iterator it = EngineApp->resources->shaders.begin(); it != EngineApp->resources->shaders.end(); ++it)
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
			for (std::map<uint, Broken::ResourceMesh*>::const_iterator it = EngineApp->resources->meshes.begin(); it != EngineApp->resources->meshes.end(); ++it)
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
			for (std::map<uint, Broken::ResourceTexture*>::const_iterator it = EngineApp->resources->textures.begin(); it != EngineApp->resources->textures.end(); ++it)
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
			for (std::map<uint, Broken::ResourceShaderObject*>::const_iterator it = EngineApp->resources->shader_objects.begin(); it != EngineApp->resources->shader_objects.end(); ++it)
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
			for (std::map<uint, Broken::ResourceMeta*>::const_iterator it = EngineApp->resources->metas.begin(); it != EngineApp->resources->metas.end(); ++it)
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

void PanelResources::DrawResourceNode(Broken::Resource* resource, ImVec4& color)
{
	ImGui::Text(resource->GetName());
	ImGui::SameLine();
	ImGui::Text("Instances: ");
	ImGui::SameLine();
	ImGui::TextColored(color, std::to_string(resource->GetNumInstances()).c_str());
}
