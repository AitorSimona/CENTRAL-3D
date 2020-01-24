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


ImporterModel::ImporterModel() : Importer(Importer::ImporterType::Model)
{
}

ImporterModel::~ImporterModel()
{
}

// --- Import external file ---
Resource* ImporterModel::Import(ImportData* IData) const
{
	// --- Import scene from path ---
	const aiScene* model = aiImportFileEx(IData->path, aiProcessPreset_TargetRealtime_MaxQuality | aiPostProcessSteps::aiProcess_FlipUVs, App->fs->GetAssimpIO());

	GameObject* rootnode = nullptr;
	ImporterScene* IScene = App->resources->GetImporter<ImporterScene>();

	if (model && IScene)
	{
		rootnode = App->scene_manager->CreateEmptyGameObject();
		//// --- Set root node name as file name with no extension ---
		//rootnode->SetName(rootnodename.data());

		// --- Save Game objects to vector so we can save to lib later ---
		std::vector<GameObject*> model_gos;
		model_gos.push_back(rootnode);

		// --- Load all meshes ---
		std::map<uint, ResourceMesh*> model_meshes;
		IScene->LoadSceneMeshes(model, model_meshes);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---
		IScene->LoadNodes(model->mRootNode,rootnode,model, model_gos, IData->path, model_meshes);

		// --- Save to Own format files in Library ---
		std::string exported_file = IScene->SaveSceneToFile(model_gos, rootnode->GetName());

		// --- Free everything ---
		IScene->FreeSceneMeshes(&model_meshes);
		rootnode->RecursiveDelete();

		// --- Free scene ---
		aiReleaseImport(model);

	}
	else
		CONSOLE_LOG("|[error]: Error loading FBX %s", &IData->path);

	return nullptr;
}


// --- Load file from library ---
Resource* ImporterModel::Load(const char* path) const
{
	return nullptr;
}