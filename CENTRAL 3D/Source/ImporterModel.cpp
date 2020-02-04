#include "ImporterModel.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneManager.h"
#include "ModuleResources.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "GameObject.h"
#include "ImporterScene.h"
#include "ImporterMeta.h"

#include "Components.h"

#include "ResourceModel.h"

#include "mmgr/mmgr.h"


ImporterModel::ImporterModel() : Importer(Importer::ImporterType::Model)
{
}

ImporterModel::~ImporterModel()
{
}

// --- Import external file ---
Resource* ImporterModel::Import(ImportData& IData) const
{
	ResourceModel* model = nullptr;

	// --- Import scene from path ---
	const aiScene* scene = aiImportFileEx(IData.path, aiProcessPreset_TargetRealtime_MaxQuality | aiPostProcessSteps::aiProcess_FlipUVs, App->fs->GetAssimpIO());

	GameObject* rootnode = nullptr;
	ImporterScene* IScene = App->resources->GetImporter<ImporterScene>();

	if (scene && IScene)
	{
		uint UID = App->GetRandom().Int();
		model = (ResourceModel*)App->resources->CreateResource(Resource::ResourceType::MODEL, IData.path);

		rootnode = App->scene_manager->CreateEmptyGameObject();
		//// --- Set root node name as file name with no extension ---
		//rootnode->SetName(rootnodename.data());

		// --- Save Game objects to vector so we can save to lib later ---
		std::vector<GameObject*> model_gos;
		model_gos.push_back(rootnode);

		// --- Load all meshes ---
		std::map<uint, ResourceMesh*> model_meshes;
		IScene->LoadSceneMeshes(scene, model_meshes, IData.path);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---
		IScene->LoadNodes(scene->mRootNode,rootnode, scene, model_gos, IData.path, model_meshes);

		// --- Save to Own format files in Library ---
		Save(model, model_gos, rootnode->GetName());

		// --- Free everything ---
		IScene->FreeSceneMeshes(&model_meshes);
		rootnode->RecursiveDelete();

		// --- Free scene ---
		aiReleaseImport(scene);

	}
	else
		CONSOLE_LOG("|[error]: Error loading FBX %s", IData.path);

	return model;
}


// --- Load file from library ---
Resource* ImporterModel::Load(const char* path) const
{
	Resource* resource = nullptr;

	if (path)
	{
		// MYTODO: Define ResourceModel and .model, should be able to retrieve original file, UID...
		resource = App->resources->CreateResourceGivenUID(Resource::ResourceType::MODEL, path, App->GetRandom().Int());


		// MYTODO: what lies below should be in Model's LoadInMemory

		//// --- Load Scene/model file ---
		//json file = App->GetJLoader()->Load(path);

		//// --- Delete buffer data ---
		//if (!file.is_null())
		//{
		//	std::vector<GameObject*> objects;

		//	// --- Iterate main nodes ---
		//	for (json::iterator it = file.begin(); it != file.end(); ++it)
		//	{
		//		// --- Create a Game Object for each node ---
		//		GameObject* go = App->scene_manager->CreateEmptyGameObject();

		//		// --- Retrieve GO's UID and name ---
		//		go->SetName(it.key().data());
		//		std::string uid = file[it.key()]["UID"];
		//		go->GetUID() = std::stoi(uid);

		//		// --- Iterate components ---
		//		json components = file[it.key()]["Components"];


		//		for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
		//		{
		//			// --- Determine ComponentType ---
		//			std::string type_string = it2.key();
		//			uint type_uint = std::stoi(type_string);
		//			Component::ComponentType type = (Component::ComponentType)type_uint;

		//			Component* component = nullptr;

		//			// --- Create/Retrieve Component ---
		//			component = go->AddComponent(type);

		//			// --- Load Component Data ---
		//			if (component)
		//				component->Load(components[type_string]);

		//		}

		//		objects.push_back(go);
		//	}

		//	// --- Parent Game Objects / Build Hierarchy ---
		//	for (uint i = 0; i < objects.size(); ++i)
		//	{
		//		std::string parent_uid_string = file[objects[i]->GetName()]["Parent"];
		//		uint parent_uid = std::stoi(parent_uid_string);
		//	
		//		for (uint j = 0; j < objects.size(); ++j)
		//		{
		//			if (parent_uid == objects[j]->GetUID())
		//			{
		//				objects[j]->AddChildGO(objects[i]);
		//			break;
		//			}
		//		}
		//	}
		//}
	}

	return resource;
}

void ImporterModel::Save(ResourceModel* model, std::vector<GameObject*>& model_gos, std::string& model_name) const
{
	// --- Save Model to file ---

	json file;

	for (int i = 0; i < model_gos.size(); ++i)
	{
		// --- Create GO Structure ---
		file[model_gos[i]->GetName()];
		file[model_gos[i]->GetName()]["UID"] = std::to_string(model_gos[i]->GetUID());
		file[model_gos[i]->GetName()]["Parent"] = std::to_string(model_gos[i]->parent->GetUID());
		file[model_gos[i]->GetName()]["Components"];

		for (int j = 0; j < model_gos[i]->GetComponents().size(); ++j)
		{
			// --- Save Components to file ---
			file[model_gos[i]->GetName()]["Components"][std::to_string((uint)model_gos[i]->GetComponents()[j]->GetType())] = model_gos[i]->GetComponents()[j]->Save();
		}

	}

	// --- Serialize JSON to string ---
	std::string data;
	data = App->GetJLoader()->Serialize(file);

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(model->GetResourceFile(), buffer, size);
}

