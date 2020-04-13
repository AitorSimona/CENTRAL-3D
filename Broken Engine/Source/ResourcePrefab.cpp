#include "ResourcePrefab.h"

// -- Modules --
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleSelection.h"

#include "Imgui/imgui.h"

#include "mmgr/mmgr.h"

using namespace Broken;

ResourcePrefab::ResourcePrefab(uint UID, const char* source_file) : Resource(Resource::ResourceType::PREFAB, UID, source_file)
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
	if (!App->gui->editingPrefab && App->GetAppState() != PLAY && ImGui::Button("Open Prefab", ImVec2(ImGui::GetWindowContentRegionWidth(), 20)))
		EditPrefab();
}

void ResourcePrefab::EditPrefab()
{
	App->gui->prefab = this;
	App->gui->openPrefab = true;

	App->selection->ClearSelection();
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
