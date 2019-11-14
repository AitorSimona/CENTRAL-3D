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
#include "ModuleFileSystem.h"

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

// MYTODO: Give some use to return type (bool) in all functions (if load fails log...)

bool ImporterScene::Import(const char * File_path, const ImportData & IData) const
{
	ImportSceneData Sdata = (ImportSceneData&) IData;

	std::string rootnodename = File_path;

	// --- Remove the directory and file type, obtaining the file name ---
	uint count = rootnodename.find_last_of("/");
	rootnodename = rootnodename.substr(count + 1, rootnodename.size());

	uint countdot = rootnodename.find_last_of(".");
	std::string extension = rootnodename.substr(countdot,rootnodename.length());
	rootnodename = rootnodename.substr(0, countdot);

	// --- Duplicate File into Assets folder, save relative path ---
	std::string relative_path = ASSETS_FOLDER;
	relative_path.append(rootnodename);
	relative_path.append(extension);


	// --- Copy File to Assets Folder ---
	App->fs->CopyFromOutsideFS(File_path, relative_path.data());

	// --- Load file from assets folder ---
	char* buffer;
	uint size = App->fs->Load(relative_path.data(), &buffer);

	// --- Import scene from path ---
	const aiScene* scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	// --- Release data ---
	delete[] buffer;


	GameObject* rootnode = App->scene_manager->CreateEmptyGameObject();

	// --- Set root node name as file name with no extension ---
	rootnode->SetName(rootnodename.data());

	if (scene != nullptr && scene->HasMeshes())
	{
		// --- Save Game objects to vector so we can save to lib later ---
		std::vector<GameObject*> scene_gos;
		scene_gos.push_back(rootnode);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---
		LoadNodes(scene->mRootNode,rootnode,scene, scene_gos, File_path);

		// --- Save to Own format files in Library ---
		std::string exported_file = SaveSceneToFile(scene_gos, rootnodename, MODEL);
		exported_file = exported_file.substr(1, exported_file.size());

		// --- Delete Everything once Library files have been created ---
		rootnode->RecursiveDelete(rootnode);

		// --- Load from Library, our own format files ---
		Load(exported_file.data());

		// --- Free scene ---
		aiReleaseImport(scene);

	}
	else
		LOG("|[error]: Error loading FBX %s", &File_path);

	return true;
}

bool ImporterScene::Load(const char * exported_file) const
{
	// --- Load Scene/model file ---
	json file = App->GetJLoader()->Load(exported_file);


	std::vector<GameObject*> objects;

	for (json::iterator it = file.begin(); it != file.end(); ++it)
	{
		// --- Create a Game Object for each node ---
		GameObject* new_go = App->scene_manager->CreateEmptyGameObject();

		// --- Retrieve GO's UID and name ---
		new_go->SetName(it.key().data());
		std::string uid = file[it.key()]["UID"];
		new_go->GetUID() = std::stoi(uid);

		// --- Iterate components ---
		json components = file[it.key()]["Components"];


		for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
		{
			// --- Determine ComponentType ---
			std::string val = it2.key();
			uint value = std::stoi(val);		
			Component::ComponentType type = (Component::ComponentType)value;

			// --- Create components to fill ---
			ComponentMesh* mesh = nullptr;
			ComponentMaterial* mat = nullptr;

			// --- Get path to component file ---
			std::string component_path = components[val];

			switch (type)
			{
				case Component::ComponentType::Renderer:
					new_go->AddComponent(Component::ComponentType::Renderer);
					break;

				case Component::ComponentType::Material:
					component_path = component_path.substr(1, component_path.size());
					mat = App->scene_manager->CreateEmptyMaterial();
					IMaterial->Load(component_path.data(), *mat);
					
					new_go->SetMaterial(mat);
					break;

				case Component::ComponentType::Mesh:
					mesh = (ComponentMesh*) new_go->AddComponent(type);
					IMesh->Load(component_path.data(), *mesh);
					break;

			}
		}


		objects.push_back(new_go);
	}

	// --- Parent GO's ---
	for (uint i = 0; i < objects.size(); ++i)
	{
		std::string parent_uid = file[objects[i]->GetName()]["Parent"];
		uint p_uid = std::stoi(parent_uid);

		for (uint j = 0; j < objects.size(); ++j)
		{
			if (p_uid == objects[j]->GetUID())
			{
				objects[j]->AddChildGO(objects[i]);
				continue;
			}
		}
	}

	return true;
}

std::string ImporterScene::SaveSceneToFile(std::vector<GameObject*>& scene_gos, std::string& scene_name, ExportFileTypes exportedfile_type) const
{
	// --- Save Scene/Model to file ---

	json file;

	for (int i = 0; i < scene_gos.size(); ++i)
	{
		// --- Create GO Structure ---
		file[scene_gos[i]->GetName()];
		file[scene_gos[i]->GetName()]["UID"] = std::to_string(scene_gos[i]->GetUID());
		file[scene_gos[i]->GetName()]["Parent"] = std::to_string(scene_gos[i]->parent->GetUID());
		file[scene_gos[i]->GetName()]["Components"];

		for (int j = 0; j < scene_gos[i]->GetComponents().size(); ++j)
		{

			std::string component_path;

			// --- Save Components to files ---

			switch (scene_gos[i]->GetComponents()[j]->GetType())
			{

				case Component::ComponentType::Transform:

					break;
				case Component::ComponentType::Mesh:
					component_path = MESHES_FOLDER;
					component_path.append(std::to_string(App->GetRandom().Int()));
					component_path.append(".mesh");
					IMesh->Save(scene_gos[i]->GetComponent<ComponentMesh>(Component::ComponentType::Mesh), component_path.data());
					break;
				case Component::ComponentType::Renderer:
					
					break;

				case Component::ComponentType::Material:
					component_path = TEXTURES_FOLDER;
					component_path.append(std::to_string(scene_gos[i]->GetComponent<ComponentMaterial>(Component::ComponentType::Material)->LibUID));
					component_path.append(".dds");
					break;
				
			}

			// --- Store path to component file ---
			file[scene_gos[i]->GetName()]["Components"][std::to_string((uint)scene_gos[i]->GetComponents()[j]->GetType())] = component_path;

		}
	}
	// --- Serialize JSON to string ---
	std::string data;
	data = App->GetJLoader()->Serialize(file);

	// --- Set destination file given exportfile type ---
	std::string path;

	switch (exportedfile_type)
	{
		case MODEL:
			path = MODELS_FOLDER;
			path.append(scene_name);
			path.append(".model");
			break;

		case SCENE:
			path = SCENES_FOLDER;
			path.append(scene_name);
			path.append(".scene");
			break;
	}

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(path.data(), buffer, size);

	return path;
}

void ImporterScene::LoadNodes(const aiNode* node, GameObject* parent, const aiScene* scene, std::vector<GameObject*>& scene_gos, const char* File_path) const
{
	// --- Load Game Objects from Assimp scene ---

	GameObject* nodeGo = nullptr;

	if (node != scene->mRootNode && node->mNumMeshes > 1)
	{
		// --- Create GO per each node that contains a mesh ---
		nodeGo = App->scene_manager->CreateEmptyGameObject();
		nodeGo->SetName(node->mName.C_Str());
		parent->AddChildGO(nodeGo);
		scene_gos.push_back(nodeGo);
	}
	else // If rootnode, set nodeGo as root
		nodeGo = parent;

	// --- Iterate children and repeat process ---
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNodes(node->mChildren[i], nodeGo,scene, scene_gos, File_path);
	}

	// --- Iterate and load meshes ---
	for (int j = 0; j < node->mNumMeshes; ++j)
	{
		// --- Create Game Object per mesh ---
		GameObject* new_object = App->scene_manager->CreateEmptyGameObject();
		new_object->SetName(node->mName.C_Str());
		nodeGo->AddChildGO(new_object);
		scene_gos.push_back(new_object);

		// --- Get Scene mesh associated to node's mesh at index ---
		uint mesh_index = node->mMeshes[j];
		aiMesh* mesh = scene->mMeshes[mesh_index];

		if (mesh)
		{

			// --- Create new Component Mesh to store current scene mesh data ---
			ComponentMesh* new_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);

			// --- Create Default components ---
			if (new_mesh)
			{
				// --- Import mesh data (fill new_mesh)---
				ImportMeshData Mdata;
				Mdata.mesh = mesh;
				Mdata.new_mesh = new_mesh;
				IMesh->Import(Mdata);

				// --- Create new Component Renderer to draw mesh ---
				ComponentRenderer* Renderer = (ComponentRenderer*)new_object->AddComponent(Component::ComponentType::Renderer);

				// --- Create new Component Material to store scene's, meshes will use this for now since we do not want to create a material for every mesh if not needed ---
				ComponentMaterial* Material = App->scene_manager->CreateEmptyMaterial();

				// --- Import Material Data (fill Material) --- 

				ImportMaterialData MData;
				MData.scene = scene;
				MData.new_material = Material;
				IMaterial->Import(File_path, MData);

				// --- Set Object's Material ---
				new_object->SetMaterial(Material);			
			}

			// --- When the mesh is loaded, frame it with the camera ---
			App->camera->FrameObject(new_object);

		}
	}
}
