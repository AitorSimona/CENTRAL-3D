#include "ModuleResourceManager.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleGui.h"
#include "ModuleTextures.h"
#include "ModuleSceneManager.h"
#include "ModuleRenderer3D.h"

#include "Importers.h"
#include "Resources.h"

#include "PanelProject.h"

#include "Assimp/include/cimport.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc142-mt.lib")

#include "mmgr/mmgr.h"

// --- Get Assimp LOGS and print them to console ---
void MyAssimpCallback(const char* msg, char* userData)
{
	CONSOLE_LOG("[Assimp]: %s", msg);
}

ModuleResourceManager::ModuleResourceManager(bool start_enabled)
{
	name = "Resource Manager";
}

ModuleResourceManager::~ModuleResourceManager()
{
}

bool ModuleResourceManager::Init(json file)
{
	// --- Stream LOG messages to MyAssimpCallback, that sends them to console ---
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	aiAttachLogStream(&stream);

    
	// --- Create importers ---
	importers.push_back(new ImporterFolder());
	importers.push_back(new ImporterScene());
	importers.push_back(new ImporterModel());
	importers.push_back(new ImporterPrefab());
	importers.push_back(new ImporterMaterial());
	importers.push_back(new ImporterShader());
	importers.push_back(new ImporterMesh());
	importers.push_back(new ImporterTexture());
	importers.push_back(new ImporterMeta());

	return true;
}

bool ModuleResourceManager::Start()
{
	// --- Import all resources in Assets at startup ---
	App->gui->CreateIcons();

	// --- Set engine's basic shaders ---
	App->renderer3D->CreateDefaultShaders();

	// --- Create default material ---
	DefaultMaterial = (ResourceMaterial*)CreateResource(Resource::ResourceType::MATERIAL, "DefaultMaterial");

	// --- Create primitives ---
	App->scene_manager->cube = (ResourceMesh*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MESH, "DefaultCube", 2);
	App->scene_manager->sphere = (ResourceMesh*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MESH, "DefaultSphere", 3);
	App->scene_manager->capsule = (ResourceMesh*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MESH, "DefaultCapsule", 4);
	App->scene_manager->plane = (ResourceMesh*)App->resources->CreateResourceGivenUID(Resource::ResourceType::MESH, "DefaultPlane", 5);

	App->scene_manager->CreateCube(1, 1, 1, App->scene_manager->cube);
	App->scene_manager->CreateSphere(1.0f, 25, 25, App->scene_manager->sphere);
	App->scene_manager->CreateCapsule(1, 1, App->scene_manager->capsule);
	App->scene_manager->CreatePlane(1, 1, 1, App->scene_manager->plane);

	App->scene_manager->cube->LoadToMemory();
	App->scene_manager->sphere->LoadToMemory();
	App->scene_manager->capsule->LoadToMemory();
	App->scene_manager->plane->LoadToMemory();

	// --- Create default scene ---
	App->scene_manager->defaultScene = (ResourceScene*)App->resources->CreateResourceGivenUID(Resource::ResourceType::SCENE, "Assets/Scenes/DefaultScene.scene", 1);
	App->scene_manager->currentScene = App->scene_manager->defaultScene;

	// --- Add file filters, so we only search for relevant files ---
	filters.push_back("fbx");
	filters.push_back("prefab");
	filters.push_back("mat");
	filters.push_back("png");
	filters.push_back("jpg");
	filters.push_back("scene");
	filters.push_back("glsl");

	// --- Import files and folders ---
	AssetsFolder = SearchAssets(nullptr, ASSETS_FOLDER, filters);

	// --- Manage changes ---
	HandleFsChanges();

	// --- Tell Windows to notify us when changes to given directory and subtree occur ---
	App->fs->WatchDirectory(ASSETS_FOLDER);


	return true;
}

// ------------------------------ IMPORTING --------------------------------------------------------

std::string ModuleResourceManager::DuplicateIntoGivenFolder(const char* path, const char* folder_path)
{
	std::string new_path = folder_path;
	std::string file;

	App->fs->SplitFilePath(path, nullptr, &file, nullptr);
	new_path.append(file);

	if (!App->fs->Exists(new_path.c_str()))
		App->fs->CopyFromOutsideFS(path, new_path.c_str());

	return new_path;
}

// --- Sweep over all files in given directory, if those files pass the given filters, call Import and if it succeeds add them to the given resource folder ---
ResourceFolder* ModuleResourceManager::SearchAssets(ResourceFolder* parent, const char* directory, std::vector<std::string>& filters)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");

	App->fs->DiscoverFiles(dir.c_str(), files, dirs);

	// --- Import folder ---
	Importer::ImportData IData(dir.c_str());
	ResourceFolder* folder = (ResourceFolder*)ImportFolder(IData);

	// --- If parent is not nullptr add ourselves as childs ---
	if (parent)
		parent->AddChild(folder);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		SearchAssets(folder,(dir + (*it) + "/").c_str(), filters);
	}

	// --- Now import all of its engine-supported files ---
	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool pass_filter = false;

		if (filters.size() > 0)
		{
			for (uint i = 0; i < filters.size(); ++i)
			{
				std::string extension = (str.substr(str.find_last_of(".") + 1));
				App->fs->NormalizePath(extension, true);

				if (extension == filters[i])
				{
					pass_filter = true;
					break;
				}
			}
		}
		else
		pass_filter = true;


		// --- If the given file has a compatible extension, try to import it ---
		if (pass_filter)
		{
			std::string path = directory;
			path.append((*it).data());
			Importer::ImportData IData2(path.c_str());
			Resource* resource = ImportAssets(IData2);
		}
	}

	return folder;
}

// --- Identify resource by file extension, call relevant importer, prepare everything for its use ---
Resource* ModuleResourceManager::ImportAssets(Importer::ImportData& IData)
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Import Switch needs to be updated");

	// --- Only standalone resources go through import here, mesh and some materials are imported through model's importer ---

	// --- Identify resource type by file extension ---
	Resource::ResourceType type = GetResourceTypeFromPath(IData.path);

	Resource* resource = nullptr;

	// --- Call relevant function depending on resource type ---

	switch (type)
	{
	case Resource::ResourceType::FOLDER:
		resource = ImportFolder(IData);
		break;

	case Resource::ResourceType::SCENE:
		resource = ImportScene(IData);
		break;

	case Resource::ResourceType::MODEL:
		resource = ImportModel(IData);
		break;

	case Resource::ResourceType::PREFAB:
		resource = ImportPrefab(IData);
		break;

	case Resource::ResourceType::MATERIAL:
		resource = ImportMaterial(IData);
		break;

	case Resource::ResourceType::SHADER:
		resource = ImportShader(IData);
		break;

	case Resource::ResourceType::TEXTURE:
		resource = ImportTexture(IData);
		break;

	case Resource::ResourceType::MESH:
		resource = ImportMesh(IData);
		break;

	case Resource::ResourceType::META:
		resource = ImportMeta(IData);
		break;

	case Resource::ResourceType::UNKNOWN:
		break;
	default:
		CONSOLE_LOG("![Warning]: Detected unsupported file type on: %s", IData.path);
		break;
	}

	if (resource)
	{
		if(type != Resource::ResourceType::META)
			AddResourceToFolder(resource);

		CONSOLE_LOG("Imported successfully: %s", IData.path);
	}
	else
		CONSOLE_LOG("![Warning]: Could not import: %s", IData.path);


	return resource;
}


Resource* ModuleResourceManager::ImportFolder(Importer::ImportData& IData)
{
	ImporterFolder* IFolder = GetImporter<ImporterFolder>();

	Resource* folder = nullptr;

	if (IFolder)
	{
		// --- Eliminate last / so we can build the meta file name ---
		std::string new_path = IData.path;
		new_path.pop_back();

		// --- If the resource is already in library, load from there ---
		if (IsFileImported(new_path.c_str()))
			folder = IFolder->Load(IData.path);

		// --- Else call relevant importer ---
		else
		{
			new_path = IData.path = new_path.append("/").c_str();

			if (IData.dropped)
				new_path = DuplicateIntoGivenFolder(IData.path, App->gui->panelProject->GetcurrentDirectory()->GetResourceFile());

			IData.path = new_path.c_str();
			folder = IFolder->Import(IData);
		}
	}

	return folder;
}

Resource* ModuleResourceManager::ImportScene(Importer::ImportData& IData)
{
	Resource* scene = nullptr;
	ImporterScene* IScene = GetImporter<ImporterScene>();

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
	{
		scene = IScene->Load(IData.path);
		//Loadfromlib
	}

	// --- Else call relevant importer ---
	else
		scene = IScene->Import(IData);


	return scene;
}

Resource* ModuleResourceManager::ImportModel(Importer::ImportData& IData)
{
	ImporterModel* IModel = GetImporter<ImporterModel>();

	Resource* model = nullptr;

	if (IModel)
	{
		// --- If the resource is already in library, just create the resource with no data ---
		if (IsFileImported(IData.path))
			model = IModel->Load(IData.path);

		// --- Else call relevant importer ---
		else
		{
			std::string new_path = IData.path;

			if (IData.dropped)
				new_path = DuplicateIntoGivenFolder(IData.path, App->gui->panelProject->GetcurrentDirectory()->GetResourceFile());

			IData.path = new_path.c_str();
			ImportModelData MData(IData.path);
			model = IModel->Import(MData);
		}
	}

	return model;
}

Resource* ModuleResourceManager::ImportPrefab(Importer::ImportData& IData)
{
	Resource* prefab = nullptr;
	ImporterPrefab* IPrefab = GetImporter<ImporterPrefab>();

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
	{
		prefab = IPrefab->Load(IData.path);
		//Loadfromlib
	}

	// --- Else call relevant importer ---
	else
		prefab = IPrefab->Import(IData);


	return prefab;
}

Resource* ModuleResourceManager::ImportMaterial(Importer::ImportData& IData)
{
	ImporterMaterial* IMat = GetImporter<ImporterMaterial>();

	Resource* material = nullptr;

	// --- If the resource is already imported (has meta), load it ---
	if (IsFileImported(IData.path))
		material = IMat->Load(IData.path);

	return material;
}


Resource* ModuleResourceManager::ImportMesh(Importer::ImportData& IData)
{
	Resource* mesh = nullptr;
	ImporterMesh* IMesh = GetImporter<ImporterMesh>();

	// --- Load the mesh directly from the lib (only declaration)---
	if (IData.path && IMesh)
	{
		mesh = IMesh->Load(IData.path);
	}


	return mesh;
}

Resource* ModuleResourceManager::ImportTexture(Importer::ImportData& IData)
{
	Resource* texture = nullptr;
	ImporterTexture* ITex = GetImporter<ImporterTexture>();

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
		texture = ITex->Load(IData.path);


	// --- Else call relevant importer ---
	else
	{
		std::string new_path = IData.path;

		if (IData.dropped)
			new_path = DuplicateIntoGivenFolder(IData.path, App->gui->panelProject->GetcurrentDirectory()->GetResourceFile());

		IData.path = new_path.c_str();
		texture = ITex->Import(IData);
	}

	return texture;
}

Resource* ModuleResourceManager::ImportShader(Importer::ImportData& IData)
{
	Resource* shader = nullptr;
	ImporterShader* IShader = GetImporter<ImporterShader>();

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
		shader = IShader->Load(IData.path);

	// --- Else call relevant importer ---
	else
	{
		std::string new_path = IData.path;

		if (IData.dropped)
			new_path = DuplicateIntoGivenFolder(IData.path, App->gui->panelProject->GetcurrentDirectory()->GetResourceFile());

		IData.path = new_path.c_str();
		shader = IShader->Import(IData);
	}

	return shader;
}


Resource* ModuleResourceManager::ImportMeta(Importer::ImportData& IData)
{
	Resource* resource = nullptr;

	ImporterMeta* IMeta = GetImporter<ImporterMeta>();

	if (IMeta)
	{
		std::string filepath = IData.path;
		filepath = filepath.substr(0, filepath.find_last_of("."));
		IData.path = filepath.c_str();

		resource = IMeta->Load(IData.path);
	}


	return resource;
}

void ModuleResourceManager::HandleFsChanges()
{
	// --- First retrieve all windows fs files and directories in ASSETS ---
	std::map<std::string, std::vector<std::string>> dirs;

	// MYTODO: Add only files/dirs that have been modified (getmoddate?)
	RetrieveFilesAndDirectories(ASSETS_FOLDER, dirs);

	// --- Now compare to engine's, we need to handle overwrite/creation/deletion ---
	// Same strategy as Unity, no support for movement/rename of elements

	for (std::map<uint, ResourceMeta*>::iterator meta = metas.begin(); meta != metas.end(); ++meta)
	{
		// --- Check if meta exists ---
		if (!App->fs->Exists((*meta).second->GetResourceFile()))
		{
			// Recreate meta, only if directory exists
			std::string metadir;
			App->fs->SplitFilePath((*meta).second->GetResourceFile(), &metadir);

			// continue only if directory exists, after recreating meta so file is not imported again
			if (App->fs->Exists(metadir.c_str()) && App->fs->Exists((*meta).second->GetOriginalFile()))
			{
				ImporterMeta* IMeta = GetImporter<ImporterMeta>();

				IMeta->Save((*meta).second);
				continue;
			}

		}

		// --- Meta's associated file has been deleted, print warning and eliminate lib files ---
		if (!App->fs->Exists((*meta).second->GetOriginalFile()))
		{
			CONSOLE_LOG("![Warning]: A meta data file (.meta) exists but its asset: '%s' cannot be found. When moving or deleting files outside the engine, please ensure that the corresponding .meta file is moved or deleted along with it.");

			// --- Eliminate all lib files ---
			Resource* resource = GetResource((*meta).second->GetUID(), false);

			if (resource && resource->GetUID() != App->scene_manager->defaultScene->GetUID()) // do not eliminate default scene
			{
				resource->OnDelete();
				delete resource;
			}


			continue;
		}

		bool found = false;

		for (std::map<std::string, std::vector<std::string>>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
		{
			for (std::vector<std::string>::iterator files = (*dir).second.begin(); files != (*dir).second.end(); ++files)
			{
				// --- If the meta corresponds to the file ---
				if ((*meta).second->GetOriginalFile() == *files)
				{
					found = true;

					// --- Check given file modification date against meta's ---
					uint date = App->fs->GetLastModificationTime((*files).c_str());

					// --- If dates are not equal, file has been overwritten ---
					if (date != (*meta).second->Date)
					{
						CONSOLE_LOG("Reimported file: %s", (*files).c_str());

						Resource* resource = GetResource((*meta).second->GetUID(), false);

						if (resource)
						{
							resource->OnOverwrite();

							// Update meta
							Resource* meta_res = (*meta).second;
							meta_res->OnOverwrite();
						}
					}

					files = (*dir).second.erase(files);

					break;
				}
			}

			std::string dir_name = std::string((*meta).second->GetOriginalFile()).append("/");

			// --- If the meta corresponds to the directory ---
			if (!found && dir_name == (*dir).first)
			{
				found = true;

				// --- Check given file modification date against meta's ---
				uint date = App->fs->GetLastModificationTime(dir_name.c_str());

				// --- If dates are not equal, dir has been overwritten ---
				if (date != (*meta).second->Date)
				{
					// --- Basically update meta, files inside will be taken care of  ---
					CONSOLE_LOG("Reimported directory: %s", dir_name.c_str());

					Resource* resource = GetResource((*meta).second->GetUID(), false);

					if (resource)
					{
						resource->OnOverwrite();

						// Update meta
						Resource* meta_res = (*meta).second;
						meta_res->OnOverwrite();
					}
				}
			}

			if (found)
				break;

		}

	}

	// ---  Delete all metas that are now orphan :( ---
	for (std::map<uint, ResourceMeta*>::iterator meta = metas.begin(); meta != metas.end();)
	{
		if (!App->fs->Exists((*meta).second->GetOriginalFile()))
		{
			Resource* meta_res = (*meta).second;
			meta = metas.erase(meta);
			meta_res->OnDelete();
			delete meta_res;
		}
		else
		{
			meta++;
		}
	}

	// --- Finally handle all new files, basically import them ---
	for (std::map<std::string, std::vector<std::string>>::iterator dir = dirs.begin(); dir != dirs.end(); ++dir)
	{
		std::string dir_name = (*dir).first;
		dir_name.pop_back();

		// --- Check if dir has a meta, if not import directory ---
		if (!App->resources->IsFileImported(dir_name.c_str()))
		{
			Importer::ImportData IData((*dir).first.c_str());
			ImportAssets(IData);
		}

		for (std::vector<std::string>::iterator files = (*dir).second.begin(); files != (*dir).second.end(); ++files)
		{
			// --- Import files ---
			if (App->fs->Exists((*files).c_str()))
			{
				Importer::ImportData IData((*files).c_str());
				ImportAssets(IData);
			}
		}
	}
}

void ModuleResourceManager::RetrieveFilesAndDirectories(const char* directory, std::map<std::string, std::vector<std::string>>& ret)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");

	App->fs->DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		RetrieveFilesAndDirectories((dir + (*it) + "/").c_str(), ret);
	}

	// --- Now iterate all of its engine-supported files ---
	std::sort(files.begin(), files.end());

	std::vector<std::string> compatible_files;

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool pass_filter = false;

		if (filters.size() > 0)
		{
			for (uint i = 0; i < filters.size(); ++i)
			{
				std::string extension = (str.substr(str.find_last_of(".") + 1));
				App->fs->NormalizePath(extension, true);

				if (extension == filters[i])
				{
					pass_filter = true;
					break;
				}
			}
		}
		else
			pass_filter = true;


		// --- If the given file does not have a compatible extension, erase it ---
		if (pass_filter)
		{
			std::string full_path = dir + (*it);
			compatible_files.push_back(full_path);
		}
	}

	// --- Add directory to list ---
	ret[dir] = compatible_files;
}


// ----------------------------------------------------

// ------------------------------ LOADING --------------------------------------------------------


// ------------------- RESOURCE HANDLING ----------------------------------------------------------

Resource* ModuleResourceManager::GetResource(uint UID, bool loadinmemory) // loadinmem is used only when absolutely needed
{
	Resource* resource = nullptr;

	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Get Switch needs to be updated");

	// To clarify: resource = condition ? value to be assigned if true : value to be assigned if false

	resource = folders.find(UID) == folders.end() ? resource : (*folders.find(UID)).second;
	resource = resource ? resource : (scenes.find(UID) == scenes.end() ? resource : (*scenes.find(UID)).second);
	resource = resource ? resource : (models.find(UID) == models.end() ? resource : (*models.find(UID)).second);
	resource = resource ? resource : (prefabs.find(UID) == prefabs.end() ? resource : (*prefabs.find(UID)).second);
	resource = resource ? resource : (materials.find(UID) == materials.end() ? resource : (*materials.find(UID)).second);
	resource = resource ? resource : (shaders.find(UID) == shaders.end() ? resource : (*shaders.find(UID)).second);
	resource = resource ? resource : (meshes.find(UID) == meshes.end() ? resource : (*meshes.find(UID)).second);
	resource = resource ? resource : (textures.find(UID) == textures.end() ? resource : (*textures.find(UID)).second);

	if (resource && loadinmemory)
		resource->LoadToMemory();
	else if (!resource)
		CONSOLE_LOG("![Warning]: Could not load: %i", UID);


	return resource;
}

Resource * ModuleResourceManager::CreateResource(Resource::ResourceType type, std::string source_file)
{
	// Note you CANNOT create a meta resource through this function, use CreateResourceGivenUID instead

	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Creation Switch needs to be updated");

	Resource* resource = nullptr;

	switch (type)
	{
	case Resource::ResourceType::FOLDER:
		resource = (Resource*)new ResourceFolder(App->GetRandom().Int(),source_file);
		folders[resource->GetUID()] = (ResourceFolder*)resource;
		break;

	case Resource::ResourceType::SCENE:
		resource = (Resource*)new ResourceScene(App->GetRandom().Int(), source_file);
		scenes[resource->GetUID()] = (ResourceScene*)resource;
		break;

	case Resource::ResourceType::MODEL:
		resource = (Resource*)new ResourceModel(App->GetRandom().Int(), source_file);
		models[resource->GetUID()] = (ResourceModel*)resource;
		break;

	case Resource::ResourceType::PREFAB:
		resource = (Resource*)new ResourcePrefab(App->GetRandom().Int(), source_file);
		prefabs[resource->GetUID()] = (ResourcePrefab*)resource;
		break;

	case Resource::ResourceType::MATERIAL:
		resource = (Resource*)new ResourceMaterial(App->GetRandom().Int(), source_file);
		materials[resource->GetUID()] = (ResourceMaterial*)resource;
		break;

	case Resource::ResourceType::SHADER:
		resource = (Resource*)new ResourceShader(App->GetRandom().Int(), source_file);
		shaders[resource->GetUID()] = (ResourceShader*)resource;
		break;

	case Resource::ResourceType::MESH:
		resource = (Resource*)new ResourceMesh(App->GetRandom().Int(), source_file);
		meshes[resource->GetUID()] = (ResourceMesh*)resource;
		break;

	case Resource::ResourceType::TEXTURE:
		resource = (Resource*)new ResourceTexture(App->GetRandom().Int(), source_file);
		textures[resource->GetUID()] = (ResourceTexture*)resource;
		break;

	case Resource::ResourceType::UNKNOWN:
		CONSOLE_LOG("![Warning]: Detected unsupported resource type");
		break;

	default:
		CONSOLE_LOG("![Warning]: Detected unsupported resource type");
		break;
	}

	return resource;
}

Resource* ModuleResourceManager::CreateResourceGivenUID(Resource::ResourceType type, std::string source_file, uint UID)
{
	Resource* resource = nullptr;

	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Creation Switch needs to be updated");


	switch (type)
	{
	case Resource::ResourceType::FOLDER:
		resource = (Resource*)new ResourceFolder(UID, source_file);
		folders[resource->GetUID()] = (ResourceFolder*)resource;
		break;

	case Resource::ResourceType::SCENE:
		resource = (Resource*)new ResourceScene(UID, source_file);
		scenes[resource->GetUID()] = (ResourceScene*)resource;
		break;

	case Resource::ResourceType::MODEL:
		resource = (Resource*)new ResourceModel(UID, source_file);
		models[resource->GetUID()] = (ResourceModel*)resource;
		break;

	case Resource::ResourceType::PREFAB:
		resource = (Resource*)new ResourcePrefab(UID, source_file);
		prefabs[resource->GetUID()] = (ResourcePrefab*)resource;
		break;

	case Resource::ResourceType::MATERIAL:
		resource = (Resource*)new ResourceMaterial(UID, source_file);
		materials[resource->GetUID()] = (ResourceMaterial*)resource;
		break;

	case Resource::ResourceType::SHADER:
		resource = (Resource*)new ResourceShader(UID, source_file);
		shaders[resource->GetUID()] = (ResourceShader*)resource;
		break;

	case Resource::ResourceType::MESH:
		resource = (Resource*)new ResourceMesh(UID, source_file);
		meshes[resource->GetUID()] = (ResourceMesh*)resource;
		break;

	case Resource::ResourceType::TEXTURE:
		resource = (Resource*)new ResourceTexture(UID, source_file);
		textures[resource->GetUID()] = (ResourceTexture*)resource;
		break;

	case Resource::ResourceType::META:
		if (metas.find(UID) == metas.end())
		{
			resource = (Resource*)new ResourceMeta(UID, source_file);
			metas[resource->GetUID()] = (ResourceMeta*)resource;
		}
		else
			resource = metas[UID];

		break;

	case Resource::ResourceType::UNKNOWN:
		CONSOLE_LOG("![Warning]: Detected unsupported resource type");
		break;

	default:
		CONSOLE_LOG("![Warning]: Detected unsupported resource type");
		break;
	}

	return resource;
}


Resource::ResourceType ModuleResourceManager::GetResourceTypeFromPath(const char* path)
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Switch needs to be updated");

	std::string extension = "";
	App->fs->SplitFilePath(path, nullptr, nullptr, &extension);
	App->fs->NormalizePath(extension, true);

	Resource::ResourceType type = Resource::ResourceType::UNKNOWN;

	type = extension == "" ? Resource::ResourceType::FOLDER : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "scene" ? Resource::ResourceType::SCENE : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "fbx" || extension == "model" ? Resource::ResourceType::MODEL : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "prefab" ? Resource::ResourceType::PREFAB : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "mat" ? Resource::ResourceType::MATERIAL : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "glsl" ? Resource::ResourceType::SHADER : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "dds" || extension == "png" || extension == "jpg" ? Resource::ResourceType::TEXTURE : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "mesh" ? Resource::ResourceType::MESH : type) : type;
	type = type == Resource::ResourceType::UNKNOWN ? (extension == "meta" ? Resource::ResourceType::META : type) : type;


	return type;
}

ResourceFolder* ModuleResourceManager::GetAssetsFolder()
{
	return AssetsFolder;
}

uint ModuleResourceManager::GetFileFormatVersion()
{
	return fileFormatVersion;
}

uint ModuleResourceManager::GetDefaultMaterialUID()
{
	return DefaultMaterial->GetUID();
}

void ModuleResourceManager::AddResourceToFolder(Resource* resource)
{
	if (resource)
	{
		// --- Manage exceptions ---
		if (resource->GetUID() == App->renderer3D->defaultShader->GetUID())
			return;

		std::string directory;
		std::string original_file;

		for (std::map<uint, ResourceFolder*>::const_iterator it = folders.begin(); it != folders.end(); ++it)
		{
			// CAREFUL when comparing strings, not putting {} below the if resulted in erroneous behaviour
			directory = App->fs->GetDirectoryFromPath(std::string(resource->GetOriginalFile()));

			if (!directory.empty())
				directory.pop_back();

			original_file = (*it).second->GetOriginalFile();

			if (!original_file.empty())
				original_file.pop_back();



			if (resource->GetType() == Resource::ResourceType::FOLDER)
			{
				App->fs->SplitFilePath(directory.c_str(), &directory);

				if (directory == (*it).second->GetOriginalFile())
				{
					ResourceFolder* folder = (ResourceFolder*)resource;
					(*it).second->AddChild(folder);
					break;
				}
			}
			else if (directory == original_file)
			{
				(*it).second->AddResource(resource);
				break;
			}

		}
	}
}

void ModuleResourceManager::RemoveResourceFromFolder(Resource* resource)
{
	if (resource)
	{
		std::string directory;
		std::string original_file;

		for (std::map<uint, ResourceFolder*>::const_iterator it = folders.begin(); it != folders.end(); ++it)
		{
			// CAREFUL when comparing strings, not putting {} below the if resulted in erroneous behaviour
			std::string path = resource->GetOriginalFile();
			directory = App->fs->GetDirectoryFromPath(path);

			if (directory.size() > 0)
				directory.pop_back();

			original_file = (*it).second->GetOriginalFile();

			if (original_file.size() > 0)
				original_file.pop_back();

			if (directory == original_file)
			{
				(*it).second->RemoveResource(resource);
			}

		}
	}
}

bool ModuleResourceManager::IsFileImported(const char* file)
{
	bool ret = false;

	if (file)
	{
		std::string path = file;

		path.append(".meta");

		// --- PhysFS will only return true if the file is inside one of the fs predefined folders! 
		//  using that on our advantage to know if a resource is imported or not ---
		ret = App->fs->Exists(path.data());
	}

	return ret;
}

std::string ModuleResourceManager::GetNewUniqueName(Resource::ResourceType type)
{
	std::string unique_name;
	uint instance = 0;

	switch (type)
	{
	case Resource::ResourceType::FOLDER:
		unique_name = "New Folder " + std::to_string(folders.size());

		for (std::map<uint, ResourceFolder*>::iterator it = folders.begin(); it != folders.end(); ++it)
		{
			if ((*it).second->GetName() == unique_name)
			{
				instance++;
				unique_name = "New Folder" + std::to_string(folders.size() + instance);
				it = folders.begin();
			}
		}

		unique_name.append("/");
		break;

	case Resource::ResourceType::SCENE:
		unique_name = "Untitled scene " + std::to_string(scenes.size());

		for (std::map<uint, ResourceScene*>::iterator it = scenes.begin(); it != scenes.end(); ++it)
		{
			if ((*it).second->GetName() == unique_name)
			{
				instance++;
				unique_name = "Untitled scene" + std::to_string(scenes.size() + instance);
				it = scenes.begin();
			}
		}

		unique_name.append(".scene");
		break;

	case Resource::ResourceType::MATERIAL:
		unique_name = "New material " + std::to_string(materials.size());

		for (std::map<uint, ResourceMaterial*>::iterator it = materials.begin(); it != materials.end(); ++it)
		{
			if ((*it).second->GetName() == unique_name)
			{
				instance++;
				unique_name = "New material" + std::to_string(materials.size() + instance);
				it = materials.begin();
			}
		}

		unique_name.append(".mat");
		break;

	case Resource::ResourceType::SHADER:
		unique_name = "New shader" + std::to_string(shaders.size());

		for (std::map<uint, ResourceShader*>::iterator it = shaders.begin(); it != shaders.end(); ++it)
		{
			if ((*it).second->GetName() == unique_name)
			{
				instance++;
				unique_name = "New shader" + std::to_string(shaders.size() + instance);
				it = shaders.begin();
			}
		}

		unique_name.append(".glsl");
		break;

	case Resource::ResourceType::UNKNOWN:
		break;

	}

	return unique_name;
}

void ModuleResourceManager::ONResourceDestroyed(Resource* resource)
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Destruction Switch needs to be updated");

	switch (resource->GetType())
	{
	case Resource::ResourceType::FOLDER:
		folders.erase(resource->GetUID());
		break;

	case Resource::ResourceType::SCENE:
		scenes.erase(resource->GetUID());
		break;

	case Resource::ResourceType::MODEL:
		models.erase(resource->GetUID());
		break;

	case Resource::ResourceType::PREFAB:
		prefabs.erase(resource->GetUID());
		break;

	case Resource::ResourceType::MATERIAL:
		materials.erase(resource->GetUID());

		// --- Tell parent model, if any ---
		if (resource->has_parent)
		{
			for (std::map<uint, ResourceModel*>::iterator it = models.begin(); it != models.end(); ++it)
			{
				std::vector<Resource*>* model_resources = (*it).second->GetResources();

				for (std::vector<Resource*>::iterator res = model_resources->begin(); res != model_resources->end(); ++res)
				{
					if ((*res)->GetUID() == resource->GetUID())
					{
						(*it).second->RemoveResource(resource);
						break;
					}
				}

			}
		}
		break;

	case Resource::ResourceType::SHADER:
		shaders.erase(resource->GetUID());
		break;

	case Resource::ResourceType::MESH:
		meshes.erase(resource->GetUID());
		break;

	case Resource::ResourceType::TEXTURE:
		textures.erase(resource->GetUID());

		// --- Tell mats ---
		for (std::map<uint, ResourceMaterial*>::iterator it = materials.begin(); it != materials.end(); ++it)
		{
			if ((*it).second->resource_diffuse && (*it).second->resource_diffuse->GetUID() == resource->GetUID())
				(*it).second->resource_diffuse = nullptr;
		}

		break;

	case Resource::ResourceType::META:
		metas.erase(resource->GetUID());
		break;

	case Resource::ResourceType::UNKNOWN:
		CONSOLE_LOG("![Warning]: Detected unsupported resource type");
		break;

	default:
		CONSOLE_LOG("![Warning]: Detected unsupported resource type");
		break;
	}

}

// ----------------------------------------------------

update_status ModuleResourceManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResourceManager::CleanUp()
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Clean Up needs to be updated");

	// --- Delete resources ---
	for (std::map<uint, ResourceFolder*>::iterator it = folders.begin(); it != folders.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = folders.erase(it);
	}

	folders.clear();

	for (std::map<uint, ResourceScene*>::iterator it = scenes.begin(); it != scenes.end();)
	{
		//it->second->FreeMemory();
		// We do not call free memory since scene's game objects have already been deleted (we have dangling pointers in scene's maps now!)
		delete it->second;
		it = scenes.erase(it);
	}

	scenes.clear();

	for (std::map<uint, ResourceModel*>::iterator it = models.begin(); it != models.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = models.erase(it);
	}

	models.clear();

	for (std::map<uint, ResourcePrefab*>::iterator it = prefabs.begin(); it != prefabs.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = prefabs.erase(it);
	}

	prefabs.clear();

	for (std::map<uint, ResourceMaterial*>::iterator it = materials.begin(); it != materials.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = materials.erase(it);
	}

	materials.clear();

	for (std::map<uint, ResourceShader*>::iterator it = shaders.begin(); it != shaders.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = shaders.erase(it);
	}

	shaders.clear();

	for (std::map<uint, ResourceMesh*>::iterator it = meshes.begin(); it != meshes.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = meshes.erase(it);
	}

	meshes.clear();

	for (std::map<uint, ResourceTexture*>::iterator it = textures.begin(); it != textures.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = textures.erase(it);
	}

	textures.clear();

	for (std::map<uint, ResourceMeta*>::iterator it = metas.begin(); it != metas.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = metas.erase(it);
	}

	metas.clear();

	// --- Delete importers ---
	for (uint i = 0; i < importers.size(); ++i)
	{
		delete importers[i];
	}

	importers.clear();

	// --- Detach assimp log stream ---
	aiDetachAllLogStreams();

	return true;
}
