#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "PanelProject.h"
#include "PanelHierarchy.h"
#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

ResourcePrefab::ResourcePrefab(uint UID, std::string source_file) : Resource(Resource::ResourceType::PREFAB, UID, source_file)
{
	extension = ".prefab";

	resource_file = source_file;
	previewTexID = App->gui->prefabTexID;
}

ResourcePrefab::~ResourcePrefab()
{
}

bool ResourcePrefab::LoadInMemory()
{
	return true;
}

void ResourcePrefab::FreeMemory()
{
}

void ResourcePrefab::CreateInspectorNode()
{
	if(ImGui::Button("Open Prefab", ImVec2(ImGui::GetWindowContentRegionWidth(),20)))
		EditPrefab();
}

void ResourcePrefab::EditPrefab()
{
	App->gui->panelHierarchy->prefab = this;
	App->gui->panelHierarchy->openPrefab = true;

	App->gui->panelProject->SetSelected(nullptr);
}

void ResourcePrefab::OnOverwrite()
{
	NotifyUsers(ResourceNotificationType::Overwrite);

}

void ResourcePrefab::OnDelete()
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
