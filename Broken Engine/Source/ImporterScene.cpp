#include "ImporterScene.h"
#include "Application.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneManager.h"

#include "ResourceScene.h"
#include "GameObject.h"
#include "ImporterMeta.h"
#include "ResourceMeta.h"

#include "mmgr/mmgr.h"

ImporterScene::ImporterScene() : Importer(Importer::ImporterType::Scene)
{

}

ImporterScene::~ImporterScene()
{

}

// MYTODO: Give some use to return type (bool) in all functions (if load fails log...)

Resource* ImporterScene::Import(ImportData& IData) const
{
	// --- Meta was deleted, just trigger a load with a new uid ---

	return Load(IData.path);
}

Resource* ImporterScene::Load(const char * path) const
{
	ResourceScene* scene = nullptr;


	// --- Load Scene file ---
	if (path)
	{
		ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
		ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

		if (meta)
		{
			scene = App->resources->scenes.find(meta->GetUID()) != App->resources->scenes.end() ? App->resources->scenes.find(meta->GetUID())->second : (ResourceScene*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SCENE, meta->GetOriginalFile(), meta->GetUID());
		}
		else
		{
			scene = (ResourceScene*)App->resources->CreateResource(Resource::ResourceType::SCENE, path);
		}

		
	}

	return scene;
}

std::string ImporterScene::SaveSceneToFile(ResourceScene* scene, std::string& scene_name) const
{
	// --- Save Scene/Model to file ---

	json file;

	for (int i = 0; i < scene->scene_gos.size(); ++i)
	{
		// --- Create GO Structure ---
		file[scene->scene_gos[i]->GetName()];
		file[scene->scene_gos[i]->GetName()]["UID"] = std::to_string(scene->scene_gos[i]->GetUID());
		file[scene->scene_gos[i]->GetName()]["Parent"] = std::to_string(scene->scene_gos[i]->parent->GetUID());
		file[scene->scene_gos[i]->GetName()]["Components"];

		for (int j = 0; j < scene->scene_gos[i]->GetComponents().size(); ++j)
		{
			// --- Save Components to file ---
			file[scene->scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene->scene_gos[i]->GetComponents()[j]->GetType())] = scene->scene_gos[i]->GetComponents()[j]->Save();
		}

	}

	// --- Serialize JSON to string ---
	std::string data;
	data = App->GetJLoader()->Serialize(file);

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(scene->GetResourceFile(), buffer, size);

	return scene_name;
}


