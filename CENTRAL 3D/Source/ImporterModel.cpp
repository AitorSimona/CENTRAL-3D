#include "ImporterModel.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleRenderer3D.h"
#include "ModuleTextures.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "GameObject.h"
#include "ImporterMeta.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"


#include "Components.h"

#include "ResourceModel.h"
#include "ResourceMeta.h"
#include "ResourceMaterial.h"

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
	ImportModelData MData = (ImportModelData&)IData;
	ResourceModel* model = nullptr;
	const aiScene* scene = nullptr;

	// --- Import scene from path ---
	if (App->fs->Exists(MData.path))
		scene = aiImportFile(MData.path, aiProcessPreset_TargetRealtime_MaxQuality);

	GameObject* rootnode = nullptr;

	if (scene)
	{
		if (MData.model_overwrite)
		{
			model = MData.model_overwrite;
		}
		else
		{
			uint UID = App->GetRandom().Int();
			model = (ResourceModel*)App->resources->CreateResource(Resource::ResourceType::MODEL, MData.path);
		}

		rootnode = App->scene_manager->CreateEmptyGameObject();
		rootnode->is_prefab_instance = true;
		rootnode->model = model;

		// --- Save Game objects to vector so we can save to lib later ---
		std::vector<GameObject*> model_gos;
		model_gos.push_back(rootnode);

		// --- Load all meshes ---
		std::map<uint, ResourceMesh*> model_meshes;
		LoadSceneMeshes(scene, model_meshes, MData.path);

		// --- Load all materials ---
		std::map<uint, ResourceMaterial*> model_mats;
		LoadSceneMaterials(scene, model_mats, MData.path, MData.library_deleted);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---
		LoadNodes(scene->mRootNode,rootnode, scene, model_gos, MData.path, model_meshes, model_mats);

		for (uint i = 0; i < model_meshes.size(); ++i)
		{
			model->AddResource(model_meshes[i]);
		}
		for (uint j = 0; j < model_mats.size(); ++j)
		{
			model->AddResource(model_mats[j]);
		}

		// --- Create preview Texture ---
		model->SetPreviewTexID(App->renderer3D->RenderSceneToTexture(model_gos, model->previewTexPath));

		// --- Save to Own format file in Library ---
		Save(model, model_gos, rootnode->GetName());

		// --- Free everything ---
		FreeSceneMaterials(&model_mats);
		FreeSceneMeshes(&model_meshes);

		// --- Delete all go data ---
		App->scene_manager->DestroyGameObject(rootnode);

		// --- Free scene ---
		aiReleaseImport(scene);

		// --- Create meta ---
		if (MData.model_overwrite == nullptr)
		{
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, IData.path, model->GetUID());

			if (meta)
				IMeta->Save(meta);
		}
	}
	else
		CONSOLE_LOG("|[error]: Error loading FBX %s", IData.path);

	return model;
}

void ImporterModel::LoadSceneMeshes(const aiScene* scene, std::map<uint, ResourceMesh*>& scene_meshes, const char* source_file) const
{
	ImporterMesh* IMesh = App->resources->GetImporter<ImporterMesh>();

	for (uint i = 0; i < scene->mNumMeshes; ++i)
	{
		ImportMeshData MData(source_file);
		MData.mesh = scene->mMeshes[i];

		// --- Else, Import mesh data (fill new_mesh) ---
		if (IMesh)
		{
			scene_meshes[i] = (ResourceMesh*)IMesh->Import(MData);
			scene_meshes[i]->SetName(scene->mMeshes[i]->mName.C_Str());

			// --- Create preview Texture ---

			std::vector<GameObject*> gos;
			gos.push_back(App->scene_manager->CreateEmptyGameObject());

			// --- Create new Component Mesh to store current scene mesh data ---
			ComponentMesh* new_mesh = (ComponentMesh*)gos[0]->AddComponent(Component::ComponentType::Mesh);
			ComponentMeshRenderer* Renderer = (ComponentMeshRenderer*)gos[0]->AddComponent(Component::ComponentType::MeshRenderer);

			// --- Assign previously loaded mesh ---
			new_mesh->resource_mesh = scene_meshes[i];

			scene_meshes[i]->FreeMemory();
			scene_meshes[i]->LoadInMemory();
			scene_meshes[i]->SetPreviewTexID(App->renderer3D->RenderSceneToTexture(gos, scene_meshes[i]->previewTexPath));

			App->scene_manager->DestroyGameObject(gos[0]);
		}

	}
}

void ImporterModel::FreeSceneMeshes(std::map<uint, ResourceMesh*>* scene_meshes) const
{
	for (std::map<uint, ResourceMesh*>::iterator it = scene_meshes->begin(); it != scene_meshes->end();)
	{
		it->second->FreeMemory();
		it = scene_meshes->erase(it);
	}

	scene_meshes->clear();
}

void ImporterModel::LoadSceneMaterials(const aiScene* scene, std::map<uint, ResourceMaterial*>& scene_mats, const char* source_file, bool library_deleted) const
{
	for (uint i = 0; i < scene->mNumMaterials; ++i)
	{
		// MYTODO: Check if mat is being overwritten...

		ImportMaterialData MatData(source_file);
		MatData.mat = scene->mMaterials[i];

		aiString material_name;

		// --- Get material's name ---
		MatData.mat->Get(AI_MATKEY_NAME, material_name);

		std::string material_destination = std::string(ASSETS_FOLDER).append(material_name.C_Str()).append(".mat");
		ImporterMaterial* IMat = App->resources->GetImporter<ImporterMaterial>();

		// --- Import material data ---
		if (IMat)
			scene_mats[i] = (ResourceMaterial*)IMat->Import(MatData);
		 
	}
}

void ImporterModel::FreeSceneMaterials(std::map<uint, ResourceMaterial*>* scene_mats) const
{
	for (std::map<uint, ResourceMaterial*>::iterator it = scene_mats->begin(); it != scene_mats->end();)
	{
		it->second->FreeMemory();
		it = scene_mats->erase(it);
	}

	scene_mats->clear();
}

void ImporterModel::LoadNodes(const aiNode* node, GameObject* parent, const aiScene* scene, std::vector<GameObject*>& scene_gos, const char* path, std::map<uint, ResourceMesh*>& scene_meshes, std::map<uint, ResourceMaterial*>& scene_mats) const
{
	// --- Load Game Objects from Assimp scene ---

	GameObject* nodeGo = nullptr;

	if (node != scene->mRootNode && node->mNumMeshes > 1)
	{
		// --- Create GO per each node that contains a mesh ---
		nodeGo = App->scene_manager->CreateEmptyGameObject();
		nodeGo->SetName(node->mName.C_Str());
		nodeGo->is_prefab_child = true;
		parent->AddChildGO(nodeGo);
		scene_gos.push_back(nodeGo);
	}
	else // If rootnode, set nodeGo as root
		nodeGo = parent;

	// --- Iterate children and repeat process ---
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNodes(node->mChildren[i], nodeGo, scene, scene_gos, path, scene_meshes, scene_mats);
	}

	// --- Iterate and load meshes ---
	for (int j = 0; j < node->mNumMeshes; ++j)
	{
		// --- Create Game Object per mesh ---
		GameObject* new_object = App->scene_manager->CreateEmptyGameObject();
		new_object->is_prefab_child = true;

		new_object->SetName(node->mName.C_Str());
		nodeGo->AddChildGO(new_object);
		scene_gos.push_back(new_object);

		// --- Get Scene mesh associated to node's mesh at index ---
		uint mesh_index = node->mMeshes[j];
		aiMesh* mesh = scene->mMeshes[mesh_index];
		uint mat_index = mesh->mMaterialIndex;

		if (mesh)
		{

			// --- Create new Component Mesh to store current scene mesh data ---
			ComponentMesh* new_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);

			// --- Assign previously loaded mesh ---
			new_mesh->resource_mesh = scene_meshes[mesh_index];

			// --- Create Default components ---
			if (new_mesh)
			{
				ComponentTransform* transform = new_object->GetComponent<ComponentTransform>();

				if (transform)
				{
					aiVector3D aiscale;
					aiVector3D aiposition;
					aiQuaternion airotation;
					node->mTransformation.Decompose(aiscale, airotation, aiposition);
					math::Quat quat;
					quat.x = airotation.x;
					quat.y = airotation.y;
					quat.z = airotation.z;
					quat.w = airotation.w;
					float3 eulerangles = quat.ToEulerXYZ() * RADTODEG;
					transform->SetPosition(aiposition.x, aiposition.y, aiposition.z);
					transform->SetRotation(eulerangles);
					transform->Scale(aiscale.x, aiscale.y, aiscale.z);
				}
				// --- Create new Component Renderer to draw mesh ---
				ComponentMeshRenderer* Renderer = (ComponentMeshRenderer*)new_object->AddComponent(Component::ComponentType::MeshRenderer);
				Renderer->material->Release();
				Renderer->material = scene_mats[mat_index];
			}

		}
	}
}


// --- Load file from library ---
Resource* ImporterModel::Load(const char* path) const
{
	ResourceModel* resource = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	resource = App->resources->models.find(meta->GetUID()) != App->resources->models.end() ? App->resources->models.find(meta->GetUID())->second : (ResourceModel*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MODEL, meta->GetOriginalFile(), meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(resource->GetOriginalFile()))
	{
		resource->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(resource);
	}

	json file;

	if(App->fs->Exists(resource->GetResourceFile()))
		file = App->GetJLoader()->Load(resource->GetResourceFile());
	else
	{
		// --- Library has been deleted, rebuild ---
		ImportModelData MData(path);
		MData.model_overwrite = resource;
		MData.library_deleted = true;
		Import(MData);
	}
	

	if (!file.is_null())
	{
		// --- Load Tex preview ---
		std::string previewTexpath = file["PreviewTexture"].is_null() ? "none" : file["PreviewTexture"];
		uint width, height = 0;

		if (previewTexpath != "none" && App->fs->Exists(previewTexpath.c_str()))
		{
			resource->previewTexPath = previewTexpath;
			resource->SetPreviewTexID(App->textures->CreateTextureFromFile(resource->previewTexPath.c_str(), width, height));
		}

		//------


		// --- Iterate main nodes ---
		for (json::iterator it = file.begin(); it != file.end(); ++it)
		{
			if (it.key() == "PreviewTexture")
				continue;

			// --- Iterate components ---
			json components = file[it.key()]["Components"];

			for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
			{
				// --- Iterate and load resources ---
				json _resources = components[it2.key()]["Resources"];

				if (!_resources.is_null())
				{
					for (json::iterator it3 = _resources.begin(); it3 != _resources.end(); ++it3)
					{
						std::string value = _resources[it3.key()]["path"];
						Importer::ImportData IData(value.c_str());
						Resource* to_Add = App->resources->ImportAssets(IData);

						if (to_Add)
						{
							// --- Give mesh a name ---
							if (to_Add->GetType() == Resource::ResourceType::MESH)
							{
								ResourceMesh* mesh = (ResourceMesh*)to_Add;

								to_Add->SetName(it.key().c_str());

								std::string meshpreviewTexpath = _resources[it3.key()]["PreviewTexture"].is_null() ? "none" : _resources[it3.key()]["PreviewTexture"];
								uint width, height = 0;

								if (meshpreviewTexpath != "none" && App->fs->Exists(mesh->previewTexPath.c_str()))
								{
									mesh->previewTexPath = meshpreviewTexpath;
									mesh->SetPreviewTexID(App->textures->CreateTextureFromFile(mesh->previewTexPath.c_str(), width, height));
								}
							}

							//to_Add->SetParent(resource);
							resource->AddResource(to_Add);
						}
					}
				}

			}

		}
	}

	return resource;
}

GameObject* ImporterModel::InstanceOnCurrentScene(const char* model_path, ResourceModel* model) const
{
	GameObject* parent = nullptr;

	if (model_path)
	{
		// --- Load Scene/model file ---
		json file = App->GetJLoader()->Load(model_path);

		// --- Delete buffer data ---
		if (!file.is_null())
		{
			std::vector<GameObject*> objects;

			// --- Iterate main nodes ---
			for (json::iterator it = file.begin(); it != file.end(); ++it)
			{
				if (it.key() == "PreviewTexture")
					continue;

				// --- Retrieve GO's UID ---
				std::string uid = it.key();

				// --- Create a Game Object for each node ---
				GameObject* go = App->scene_manager->CreateEmptyGameObjectGivenUID(std::stoi(uid));

				// --- Set prefab bool ---
				if (!file[it.key()]["PrefabChild"].is_null())
					go->is_prefab_child = file[it.key()]["PrefabChild"];

				if (!file[it.key()]["PrefabInstance"].is_null())
				{
					go->is_prefab_instance = file[it.key()]["PrefabInstance"];

					if(go->is_prefab_instance)
						parent = go;
				}

				if (!file[it.key()]["Model"].is_null())
					go->model = (ResourceModel*)App->resources->ImportAssets(ImportData(file[it.key()]["Model"].get<std::string>().c_str()));

				// --- Retrieve GO's name ---
				std::string name = file[it.key()]["Name"];
				go->SetName(name.c_str());

				// --- Iterate components ---
				json components = file[it.key()]["Components"];


				for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2)
				{
					// --- Determine ComponentType ---
					std::string stringuid = it2.key();
					uint uid = std::stoi(stringuid);
					uint type_uint = 0;

					if (!components[it2.key()]["Type"].is_null())
						type_uint = components[it2.key()]["Type"].get<uint>();

					Component::ComponentType type = (Component::ComponentType)type_uint;
					Component* component = nullptr;

					// --- Create/Retrieve Component ---
					component = go->AddComponent(type);

					// --- Load Component Data ---
					if (component)
					{
						component->SetUID(uid);
						component->Load(components[it2.key()]);
					}

				}

				objects.push_back(go);
			}

			// --- Parent Game Objects / Build Hierarchy ---
			for (uint i = 0; i < objects.size(); ++i)
			{
				std::string parent_uid_string = file[std::to_string(objects[i]->GetUID())]["Parent"];
				uint parent_uid = std::stoi(parent_uid_string);

				for (uint j = 0; j < objects.size(); ++j)
				{
					if (parent_uid == objects[j]->GetUID())
					{
						objects[j]->AddChildGO(objects[i]);
						break;
					}
				}
			}

			// --- Now give unique uids to all gos so they are brand new ---
			for (uint i = 0; i < objects.size(); ++i)
			{
				objects[i]->SetUID(App->GetRandom().Int());
			}

			// --- Add pointer to model ---
			if (parent && model)
			{
				parent->model = model;
				model->AddUser(parent);
			}
		}
	}

	return parent;
}

void ImporterModel::Save(ResourceModel* model, std::vector<GameObject*>& model_gos, std::string& model_name) const
{
	// --- Save Model to file ---

	json file;

	file["PreviewTexture"] = model->previewTexPath;

	for (int i = 0; i < model_gos.size(); ++i)
	{
		// --- Create GO Structure ---
		file[std::to_string(model_gos[i]->GetUID())];
		file[std::to_string(model_gos[i]->GetUID())]["Name"] = model_gos[i]->GetName();
		file[std::to_string(model_gos[i]->GetUID())]["Parent"] = std::to_string(model_gos[i]->parent->GetUID());
		file[std::to_string(model_gos[i]->GetUID())]["Components"];
		file[std::to_string(model_gos[i]->GetUID())]["PrefabChild"] = model_gos[i]->is_prefab_child;
		file[std::to_string(model_gos[i]->GetUID())]["PrefabInstance"] = model_gos[i]->is_prefab_instance;

		if(model_gos[i]->model)
			file[std::to_string(model_gos[i]->GetUID())]["Model"] = std::string(model_gos[i]->model->GetOriginalFile());

		std::vector<Component*> go_components = model_gos[i]->GetComponents();

		for (int j = 0; j < go_components.size(); ++j)
		{
			// --- Save Components to file ---
			file[std::to_string(model_gos[i]->GetUID())]["Components"][std::to_string(go_components[j]->GetUID())] = go_components[j]->Save();
			file[std::to_string(model_gos[i]->GetUID())]["Components"][std::to_string(go_components[j]->GetUID())]["Type"] = (uint)go_components[j]->GetType();
		}

	}

	// --- Serialize JSON to string ---
	std::string data;
	App->GetJLoader()->Serialize(file, data);

	// --- Finally Save to file ---
	char* buffer = (char*)data.data();
	uint size = data.length();

	App->fs->Save(model->GetResourceFile(), buffer, size);
}



