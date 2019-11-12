#include "ImporterScene.h"
#include "Application.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


#include "ComponentMaterial.h"
#include "ComponentMesh.h"
#include "ComponentRenderer.h"
#include "ModuleSceneManager.h"
#include "ModuleCamera3D.h"
#include "GameObject.h"

#include "ImporterMesh.h"
#include "ImporterMaterial.h"

#include "mmgr/mmgr.h"

ImporterScene::ImporterScene() 
{
	IMesh = new ImporterMesh;
	IMaterial = new ImporterMaterial;
}

ImporterScene::~ImporterScene()
{
	delete IMesh;
	delete IMaterial;
}

bool ImporterScene::Import(const char * File_path, const ImportData & IData) const
{
	ImportSceneData Sdata = (ImportSceneData&) IData;
	
	// --- Import scene from path ---
	const aiScene* scene = aiImportFile(File_path, aiProcessPreset_TargetRealtime_MaxQuality);

	GameObject* rootnode = App->scene_manager->CreateEmptyGameObject();

	std::string rootnodename = File_path;

	// --- Remove the directory and file type, obtaining the file name ---
	uint count = rootnodename.find_last_of("/");
	rootnodename = rootnodename.substr(count + 1, rootnodename.size());

	uint countdot = rootnodename.find_last_of(".");
	rootnodename = rootnodename.substr(0, countdot);

	rootnode->SetName(rootnodename.data());

	if (scene != nullptr && scene->HasMeshes())
	{

		// --- Create new Component Material to store scene's, meshes will use this for now since we do not want to create a material for every mesh if not needed ---
		ComponentMaterial* Material = App->scene_manager->CreateEmptyMaterial();

		// --- Import Material Data (fill Material) --- 
		ImportMaterialData MData;
		MData.scene = scene;
		MData.new_material = Material;
		IMaterial->Import(File_path, MData);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---

		LoadNodes(scene->mRootNode,rootnode,scene, Material);

		// --- Free scene ---
		aiReleaseImport(scene);

	}
	else
		LOG("|[error]: Error loading FBX %s", &File_path);

	return true;
}

void ImporterScene::LoadNodes(const aiNode* node, GameObject* parent, const aiScene* scene, ComponentMaterial* Material) const
{
	GameObject* nodeGo = nullptr;

	if (node != scene->mRootNode && node->mNumMeshes > 1)
	{
		nodeGo = App->scene_manager->CreateEmptyGameObject();
		nodeGo->SetName(node->mName.C_Str());
		parent->AddChildGO(nodeGo);
	}
	else
		nodeGo = parent;

	for (int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNodes(node->mChildren[i], nodeGo,scene, Material);
	}

	for (int j = 0; j < node->mNumMeshes; ++j)
	{
		// --- Create Game Object ---
		GameObject* new_object = App->scene_manager->CreateEmptyGameObject();
		new_object->SetName(node->mName.C_Str());
		nodeGo->AddChildGO(new_object);

		// --- Get Scene mesh number i ---
		uint mesh_index = node->mMeshes[j];
		aiMesh* mesh = scene->mMeshes[mesh_index];

		if (mesh)
		{


			// --- Create new Component Mesh to store current scene mesh data ---
			ComponentMesh* new_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);

			if (new_mesh)
			{
				// --- Import mesh data (fill new_mesh)---
				ImportMeshData Mdata;
				Mdata.mesh = mesh;
				Mdata.new_mesh = new_mesh;
				IMesh->Import(Mdata);

				// --- Create new Component Renderer to draw mesh ---
				ComponentRenderer* Renderer = (ComponentRenderer*)new_object->AddComponent(Component::ComponentType::Renderer);

				if (Material)
				{
					// --- Set Object's Material ---
					new_object->SetMaterial(Material);
				}

			}

			// --- When the mesh is loaded, frame it with the camera ---
			App->camera->FrameObject(new_object);

		}
	}
}
