#include "ImporterModel.h"

// -- Modules --
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleSceneManager.h"
#include "ModuleResourceManager.h"
#include "ModuleTextures.h"
#include "ModuleRenderer3D.h"

// -- Assimp --
#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

// -- Components --
#include "GameObject.h"
#include "Components.h"

// -- Resources --
#include "ResourceModel.h"
#include "ResourceMeta.h"
#include "ResourceMaterial.h"
#include "ResourceAnimation.h"
#include "ResourceMesh.h"
#include "ResourceBone.h"

// -- Importers --
#include "ImporterMeta.h"
#include "ImporterMesh.h"
#include "ImporterMaterial.h"
#include "ImporterBone.h"
#include "ImporterAnimation.h"
#include "ImporterBone.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ImporterModel::ImporterModel() : Importer(Importer::ImporterType::Model) {
}

ImporterModel::~ImporterModel() {
}

// --- Import external file ---
Resource* ImporterModel::Import(ImportData& IData) const {
	ImportModelData MData = (ImportModelData&)IData;
	ResourceModel* model = nullptr;
	const aiScene* scene = nullptr;

	// --- Import scene from path ---
	if (App->fs->Exists(MData.path))
		scene = aiImportFile(MData.path, aiProcessPreset_TargetRealtime_MaxQuality
		| aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	GameObject* rootnode = nullptr;

	if (scene) {
		if (MData.model_overwrite) {
			model = MData.model_overwrite;
		}
		else {
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

		// --- Load animation ---
		std::map<uint, ResourceAnimation*> anims;
		LoadSceneAnimations(scene, rootnode, anims, MData.path);

		// --- Load all materials ---
		std::map<uint, ResourceMaterial*> model_mats;
		LoadSceneMaterials(scene, model_mats, MData.path, MData.library_deleted);

		std::vector<aiMesh*> mesh_collector;
		std::vector<GameObject*> mesh_wbones;

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array --
		if (scene->mRootNode->mNumChildren > 0)
		{
			for (int i = 0; i < scene->mRootNode->mNumChildren; ++i)
			{
				LoadNodes(scene->mRootNode->mChildren[i], rootnode, scene, model_gos, MData.path, model_meshes, model_mats, mesh_collector, mesh_wbones);
			}
		}

		// --- Load bones ---
		std::map<uint, ResourceBone*> bones;
		
		LoadSceneBones(mesh_collector, mesh_wbones, bones, MData.path);

		LoadBones(model_gos, mesh_collector, bones);

		for (uint i = 0; i < model_meshes.size(); ++i) {
			model->AddResource(model_meshes[i]);
		}
		for (uint j = 0; j < model_mats.size(); ++j) {
			model->AddResource(model_mats[j]);
		}
		for (uint j = 0; j < anims.size(); ++j)
		{
			model->AddResource(anims[j]);
		}
		for (uint j = 0; j < bones.size(); ++j)
		{
			model->AddResource(bones[j]);
		}

		// --- Create preview Texture ---
		uint texID = 0;
		model->previewTexPath = App->renderer3D->RenderSceneToTexture(model_gos, texID);
		model->SetPreviewTexID(texID);

		// --- Save to Own format file in Library ---
		Save(model, model_gos, rootnode->GetName());

		// --- Free everything ---
		FreeSceneMaterials(&model_mats);
		FreeSceneMeshes(&model_meshes);
		FreeSceneBones(&bones);
		FreeSceneAnimations(&anims);

		App->scene_manager->DestroyGameObject(rootnode);

		// --- Free scene ---
		aiReleaseImport(scene);

		// --- Create meta ---
		if (MData.model_overwrite == nullptr) {
			ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();

			ResourceMeta* meta = (ResourceMeta*)App->resources->CreateResourceGivenUID(Resource::ResourceType::META, IData.path, model->GetUID());

			if (meta)
				IMeta->Save(meta);
		}
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Error loading FBX %s", IData.path);

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

			//// --- Create preview Texture ---

			//std::vector<GameObject*> gos;
			//gos.push_back(App->scene_manager->CreateEmptyGameObject());

			//// --- Create new Component Mesh to store current scene mesh data ---
			//ComponentMesh* new_mesh = (ComponentMesh*)gos[0]->AddComponent(Component::ComponentType::Mesh);
			//ComponentMeshRenderer* Renderer = (ComponentMeshRenderer*)gos[0]->AddComponent(Component::ComponentType::MeshRenderer);

			//// --- Assign previously loaded mesh ---
			//new_mesh->resource_mesh = scene_meshes[i];

			scene_meshes[i]->FreeMemory();
			scene_meshes[i]->LoadInMemory();
			//uint TexID = 0;
			//std::string previewTexPath = App->renderer3D->RenderSceneToTexture(gos, TexID);
			//scene_meshes[i]->previewTexPath = previewTexPath;
			//scene_meshes[i]->SetPreviewTexID(TexID);

			//App->scene_manager->DestroyGameObject(gos[0]);
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

void ImporterModel::FreeSceneBones(std::map<uint, ResourceBone*>* scene_bones) const
{
	for (std::map<uint, ResourceBone*>::iterator it = scene_bones->begin(); it != scene_bones->end();)
	{
		it->second->FreeMemory();
		it = scene_bones->erase(it);
	}

	scene_bones->clear();
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

		if (App->fs->Exists(material_destination.c_str())) {
			scene_mats[i] = (ResourceMaterial*)IMat->Load(material_destination.c_str());
		}
		else {
			// --- Import material data ---
			if (IMat)
				scene_mats[i] = (ResourceMaterial*)IMat->Import(MatData);
		}
	}
}

void ImporterModel::LoadSceneBones(std::vector<aiMesh*>& mesh, std::vector<GameObject*>& mesh_wbones, std::map<uint, ResourceBone*>& bones, const char* source_file) const
{
	int i = 0;
	for (std::vector<aiMesh*>::iterator it = mesh.begin(); it != mesh.end(); it++)
	{
		
		if ((*it)->HasBones())
		{
			ImporterBone* IBone = App->resources->GetImporter<ImporterBone>();

			for (int j = 0; j < (*it)->mNumBones; j++)
			{
				ImportBoneData BData(source_file);
				BData.bone = (*it)->mBones[j];

				ComponentMesh* comp_mesh = (ComponentMesh*)mesh_wbones[i]->GetComponent<ComponentMesh>();
				BData.mesh_UID = comp_mesh->resource_mesh->GetUID();

				if (IBone)
				{
					bones[j] = ((ResourceBone*)IBone->Import(BData));
					bones[j]->SetName((*it)->mBones[j]->mName.C_Str());
					
				}
			}
		}
		i++;
	}

}

void ImporterModel::LoadBones(std::vector<GameObject*> model_gos, std::vector<aiMesh*> mesh_collect, std::map<uint, ResourceBone*> bones) const
{
	//GameObjects map
	std::map<std::string, GameObject*> go_map;
	for (std::vector<GameObject*>::iterator it = model_gos.begin(); it != model_gos.end(); ++it)
	{
		go_map[(*it)->GetName()] = (*it);
	}

	//ResourceBone map
	std::map<std::string, ResourceBone*> bone_map;
	for (int i=0; i<bones.size(); i++)
	{
		bone_map[bones[i]->GetName()] = bones[i];
	}

	for (int i = 0; i < mesh_collect.size(); i++)
	{
		for (int j = 0; j < mesh_collect[i]->mNumBones; j++)
		{
			//Search for GO that are bones
			std::map<std::string, GameObject*>::iterator bone = go_map.find(mesh_collect[i]->mBones[j]->mName.C_Str());
			if (bone != go_map.end())
			{
				ComponentBone* comp_bone = (ComponentBone*)bone->second->AddComponent(Component::ComponentType::Bone);

				std::map<std::string, ResourceBone*>::iterator resource_bone = bone_map.find(bone->second->GetName());
				if (resource_bone != bone_map.end())
					comp_bone->res_bone = resource_bone->second;
			}
		}
	}

}

void ImporterModel::LoadSceneAnimations(const aiScene* scene, GameObject* GO, std::map<uint, ResourceAnimation*>& anim, const char* source_file) const
{
	if (scene->HasAnimations())
	{
		ImporterAnimation* IAnim = App->resources->GetImporter<ImporterAnimation>();
		ComponentAnimation* comp_anim = (ComponentAnimation*)GO->AddComponent(Component::ComponentType::Animation);

		for (int i = 0; i < scene->mNumAnimations; i++)
		{
			ImportAnimationData AData(source_file);
			AData.animation = scene->mAnimations[i];
			

			if (IAnim)
			{
				anim[i] = (ResourceAnimation*)IAnim->Import(AData);
				comp_anim->res_anim = anim[i];
			}
		}
	}
}

void ImporterModel::FreeSceneAnimations(std::map<uint, ResourceAnimation*>* anims) const
{

	for (std::map<uint, ResourceAnimation*>::iterator it = anims->begin(); it != anims->end();)
	{
		it->second->FreeMemory();
		it = anims->erase(it);
	}

	anims->clear();

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

void ImporterModel::LoadNodes(const aiNode* node, GameObject* parent, const aiScene* scene, std::vector<GameObject*>& scene_gos, const char* path, std::map<uint, ResourceMesh*>& scene_meshes, std::map<uint, ResourceMaterial*>& scene_mats, std::vector<aiMesh*> &mesh_collector, std::vector<GameObject*>& mesh_wbones) const
{
	// --- Load Game Objects from Assimp scene ---

	aiVector3D translation, scaling;
	aiQuaternion rotation;

	node->mTransformation.Decompose(scaling, rotation, translation);

	float3 pos2(translation.x, translation.y, translation.z);
	float3 s2(scaling.x, scaling.y, scaling.z);
	Quat rot2(rotation.x, rotation.y, rotation.z, rotation.w);

	std::string node_name = node->mName.C_Str();

	bool dummyFound = true;
	while (dummyFound)//Skipp all dummy modules. Assimp loads this fbx nodes to stack all transformations
	{
		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (node_name.find("_$AssimpFbx$_") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next GameObject
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(scaling, rotation, translation);
			pos2 += float3(translation.x, translation.y, translation.z);
			s2 = float3(s2.x * scaling.x, s2.y * scaling.y, s2.z * scaling.z);
			rot2 = rot2 * Quat(rotation.x, rotation.y, rotation.z, rotation.w);

			node_name = node->mName.C_Str();

			//if we find a dummy node we "change" our current node into the dummy one and search
			//for other dummy nodes inside that one.
			dummyFound = true;
		}
		else
			dummyFound = false;
	}

	GameObject* nodeGo = nullptr;

	// --- Create GO per each node that contains a mesh ---
	nodeGo = App->scene_manager->CreateEmptyGameObject();
	nodeGo->SetName(node->mName.C_Str());
	parent->AddChildGO(nodeGo);
	scene_gos.push_back(nodeGo);
	nodeGo->is_prefab_child = true;


	ComponentTransform* transform = nodeGo->GetComponent<ComponentTransform>();

	transform->position = pos2;;
	transform->scale = s2;
	transform->rotation = rot2;
	//transform->UpdateLocalTransform();


	int i = 0;

	// --- Iterate and load meshes ---
	for (int j = 0; j < node->mNumMeshes; ++j) {
		// --- Create Game Object per mesh ---
		GameObject* new_object = nullptr;

		if (node->mNumMeshes > 1)
		{
			node_name = scene->mMeshes[node->mMeshes[j]]->mName.C_Str();
			if (node_name == "")
				node_name = std::string(nodeGo->GetName()) + "dummy";
			if (j > 0)
				node_name = std::string(nodeGo->GetName()) + "_submesh";
			new_object = App->scene_manager->CreateEmptyGameObject();
			nodeGo->AddChildGO(new_object);
			new_object->SetName(node_name.c_str());
			new_object->is_prefab_child = true;
		}
		else
		{
			//If node has only 1 mesh child is the parent itself 
			new_object = nodeGo;
		}

		//new_object->SetName(node->mName.C_Str());
		//parent->AddChildGO(new_object);
		scene_gos.push_back(new_object);

		// --- Get Scene mesh associated to node's mesh at index ---
		uint mesh_index = node->mMeshes[j];
		aiMesh* mesh = scene->mMeshes[mesh_index];
		uint mat_index = mesh->mMaterialIndex;

		if (scene->mMeshes[node->mMeshes[j]]->HasBones())
		{
			mesh_collector.push_back(scene->mMeshes[node->mMeshes[j]]);
			mesh_wbones.push_back(new_object);
		}

		if (mesh)
		{
			// --- Create new Component Mesh to store current scene mesh data ---
			ComponentMesh* new_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);

			// --- Assign previously loaded mesh ---
			new_mesh->resource_mesh = scene_meshes[mesh_index];

			// --- Create Default components ---
			if (new_mesh)
			{
				// --- Create new Component Renderer to draw mesh ---
				ComponentMeshRenderer* Renderer = (ComponentMeshRenderer*)new_object->AddComponent(Component::ComponentType::MeshRenderer);
				Renderer->material->Release();
				Renderer->material = scene_mats[mat_index];
			}

		}
	}

	// --- Iterate children and repeat process ---
	for (int i = 0; i < node->mNumChildren; ++i)
	{
		LoadNodes(node->mChildren[i], nodeGo, scene, scene_gos, path, scene_meshes, scene_mats, mesh_collector, mesh_wbones);
	}
	
}


// --- Load file from library ---
Resource* ImporterModel::Load(const char* path) const {
	ResourceModel* resource = nullptr;

	ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
	ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

	resource = App->resources->models.find(meta->GetUID()) != App->resources->models.end() ? App->resources->models.find(meta->GetUID())->second : (ResourceModel*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MODEL, meta->GetOriginalFile(), meta->GetUID());

	// --- A folder has been renamed ---
	if (!App->fs->Exists(resource->GetOriginalFile())) {
		resource->SetOriginalFile(path);
		meta->SetOriginalFile(path);
		App->resources->AddResourceToFolder(resource);
	}

	json file;

	if (App->fs->Exists(resource->GetResourceFile()))
		file = App->GetJLoader()->Load(resource->GetResourceFile());
	else {
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
						std::string value;
						if (_resources[it3.key()].is_structured())
							value = _resources[it3.key()]["path"].get<std::string>();
						else
							value = _resources[it3.key()].get<std::string>();
						Importer::ImportData IData(value.c_str());
						Resource* to_Add = App->resources->ImportAssets(IData);

						if (to_Add)
						{
							// --- Give mesh a name ---
							if (to_Add->GetType() == Resource::ResourceType::MESH)
							{
								ResourceMesh* mesh = (ResourceMesh*)to_Add;

								to_Add->SetName(it.key().c_str());

								/*std::string meshpreviewTexpath = _resources[it3.key()]["PreviewTexture"].is_null() ? "none" : _resources[it3.key()]["PreviewTexture"];
								uint width, height = 0;

								if (meshpreviewTexpath != "none" && App->fs->Exists(mesh->previewTexPath.c_str()))
								{
									mesh->previewTexPath = meshpreviewTexpath;
									mesh->SetPreviewTexID(App->textures->CreateTextureFromFile(mesh->previewTexPath.c_str(), width, height));
								}*/
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

GameObject* ImporterModel::InstanceOnCurrentScene(const char* model_path, ResourceModel* model) const {
	
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
				std::string uid = file[it.key()]["UID"];

				// --- Create a Game Object for each node ---
				GameObject* go = App->scene_manager->CreateEmptyGameObjectGivenUID(std::stoi(uid));
	
				// --- Set prefab bool ---
				if (!file[it.key()]["PrefabChild"].is_null())
					go->is_prefab_child = file[it.key()]["PrefabChild"];

				if (!file[it.key()]["PrefabInstance"].is_null())
				{
					go->is_prefab_instance = file[it.key()]["PrefabInstance"];
					parent = go;
				}

				if (!file[it.key()]["Model"].is_null())
				{
					std::string model_path = file[it.key()]["Model"];
					ImportData IData(model_path.c_str());
					go->model = (ResourceModel*)App->resources->ImportAssets(IData);
				}

				// --- Retrieve GO's name ---
				go->SetName(it.key().c_str());
	
				// --- Iterate components ---
				json components = file[it.key()]["Components"];


				for (json::iterator it2 = components.begin(); it2 != components.end(); ++it2) 
				{
					// --- Determine ComponentType ---
					std::string type_string = it2.key();
					uint type_uint = std::stoi(type_string);
					Component::ComponentType type = (Component::ComponentType)type_uint;

					Component* component = nullptr;

					// --- Create/Retrieve Component ---
					component = go->AddComponent(type);

					// --- Load Component Data ---
					if (component)
						component->Load(components[type_string]);

				}

				objects.push_back(go);
			}


			// --- Parent Game Objects / Build Hierarchy ---
			for (uint i = 0; i < objects.size(); ++i) {
				std::string parent_uid_string = file[objects[i]->GetName()]["Parent"];
				uint parent_uid = std::stoi(parent_uid_string);

				for (uint j = 0; j < objects.size(); ++j) {
					if (parent_uid == objects[j]->GetUID()) {
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

void ImporterModel::Save(ResourceModel* model, std::vector<GameObject*>& model_gos, const std::string& model_name) const {
	// --- Save Model to file ---

	json file;

	file["PreviewTexture"] = model->previewTexPath;

	for (int i = 0; i < model_gos.size(); ++i)
	{
		// --- Create GO Structure ---
		file[model_gos[i]->GetName()];
		file[model_gos[i]->GetName()]["UID"] = std::to_string(model_gos[i]->GetUID());
		file[model_gos[i]->GetName()]["Parent"] = std::to_string(model_gos[i]->parent->GetUID());
		file[model_gos[i]->GetName()]["Components"];
		file[model_gos[i]->GetName()]["PrefabChild"] = model_gos[i]->is_prefab_child;
		file[model_gos[i]->GetName()]["PrefabInstance"] = model_gos[i]->is_prefab_instance;

		if (model_gos[i]->model)
			file[model_gos[i]->GetName()]["Model"] = std::string(model_gos[i]->model->GetOriginalFile());


		for (int j = 0; j < model_gos[i]->GetComponents().size(); ++j) 
		{
			// --- Save Components to file ---
			file[model_gos[i]->GetName()]["Components"][std::to_string((uint)model_gos[i]->GetComponents()[j]->GetType())] = model_gos[i]->GetComponents()[j]->Save();
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


