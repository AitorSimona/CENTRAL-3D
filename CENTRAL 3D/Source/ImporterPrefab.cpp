#include "ImporterPrefab.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneManager.h"
#include "ModuleTextures.h"

#include "ImporterMeta.h"
#include "ResourceMeta.h"
#include "ResourcePrefab.h"
#include "ResourceModel.h"

#include "GameObject.h"

#include "mmgr/mmgr.h"


ImporterPrefab::ImporterPrefab() : Importer(Importer::ImporterType::Prefab)
{
}

ImporterPrefab::~ImporterPrefab()
{
}

Resource* ImporterPrefab::Import(ImportData& IData) const
{
	return nullptr;
}

Resource* ImporterPrefab::Load(const char* path) const
{
	ResourcePrefab* prefab = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	prefab = App->resources->prefabs.find(meta->GetUID()) != App->resources->prefabs.end() ? (ResourcePrefab*)App->resources->GetResource(meta->GetUID()) : (ResourcePrefab*)App->resources->CreateResourceGivenUID(Resource::ResourceType::PREFAB, path, meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(prefab->GetOriginalFile()))
	{
		prefab->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(prefab);
	}

	json file;

	if (App->fs->Exists(prefab->GetResourceFile()))
		file = App->GetJLoader()->Load(prefab->GetResourceFile());

	if (!file.is_null())
	{
		// --- Load Tex preview ---
		std::string previewTexpath = file["PreviewTexture"].is_null() ? "none" : file["PreviewTexture"];
		uint width, height = 0;

		if (previewTexpath != "none" && App->fs->Exists(previewTexpath.c_str()))
		{
			prefab->previewTexPath = previewTexpath;
			prefab->SetPreviewTexID(App->textures->CreateTextureFromFile(prefab->previewTexPath.c_str(), width, height));
		}
	}

	return prefab;
}

void ImporterPrefab::Save(ResourcePrefab* prefab) const
{
	if (prefab && prefab->parentgo)
	{
		// --- Get all game objects inside parent ---
		std::vector<GameObject*> prefab_gos;
		App->scene_manager->GatherGameObjects(prefab->parentgo, prefab_gos);

		// --- Give new UIDs to all gos ---
		std::vector<uint> gos;

		for (uint i = 0; i < prefab_gos.size(); ++i)
		{
			gos.push_back(prefab_gos[i]->GetUID());
			prefab_gos[i]->GetUID() = App->GetRandom().Int(); // force new uid
		}

		// --- Save Prefab to file ---

		json file;

		file["PreviewTexture"] = prefab->previewTexPath;

		for (int i = 0; i < prefab_gos.size(); ++i)
		{
			// --- Create GO Structure ---
			file[std::to_string(prefab_gos[i]->GetUID())];
			file[std::to_string(prefab_gos[i]->GetUID())]["Name"] = prefab_gos[i]->GetName();
			file[std::to_string(prefab_gos[i]->GetUID())]["Parent"] = std::to_string(prefab_gos[i]->parent->GetUID());
			file[std::to_string(prefab_gos[i]->GetUID())]["Components"];
			file[std::to_string(prefab_gos[i]->GetUID())]["PrefabChild"] = prefab_gos[i]->is_prefab_child;
			file[std::to_string(prefab_gos[i]->GetUID())]["PrefabInstance"] = prefab_gos[i]->is_prefab_instance;

			if (prefab_gos[i]->model)
				file[std::to_string(prefab_gos[i]->GetUID())]["Model"] = std::string(prefab_gos[i]->model->GetOriginalFile());

			std::vector<Component*> go_components = prefab_gos[i]->GetComponents();

			for (int j = 0; j < go_components.size(); ++j)
			{
				// --- Save Components to file ---
				file[std::to_string(prefab_gos[i]->GetUID())]["Components"][std::to_string(go_components[j]->GetUID())] = go_components[j]->Save();
				file[std::to_string(prefab_gos[i]->GetUID())]["Components"][std::to_string(go_components[j]->GetUID())]["Type"] = (uint)go_components[j]->GetType();
			}

		}

		for (uint i = 0; i < gos.size(); ++i)
		{
			prefab_gos[i]->GetUID() = gos[i]; // force new uid
		}

		// --- Serialize JSON to string ---
		std::string data;
		App->GetJLoader()->Serialize(file, data);

		// --- Finally Save to file ---
		char* buffer = (char*)data.data();
		uint size = data.length();

		App->fs->Save(prefab->GetResourceFile(), buffer, size);
	}
}
