#include "ModuleResources.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleGui.h"

#include "Importers.h"
#include "Resources.h"

#include "Assimp/include/cimport.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"

// --- Get Assimp LOGS and print them to console ---
void MyAssimpCallback(const char* msg, char* userData)
{
	CONSOLE_LOG("[Assimp]: %s", msg);
}

ModuleResources::ModuleResources(bool start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(json file)
{
	// --- Stream LOG messages to MyAssimpCallback, that sends them to console ---
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	aiAttachLogStream(&stream);

	// --- Create importers ---
	importers.push_back(new ImporterFolder());
	importers.push_back(new ImporterScene());
	importers.push_back(new ImporterModel());
	importers.push_back(new ImporterMaterial());
	importers.push_back(new ImporterShader());
	importers.push_back(new ImporterMesh());
	importers.push_back(new ImporterTexture());
	importers.push_back(new ImporterMeta());

	return true;
}

bool ModuleResources::Start()
{
	// --- Import all resources in Assets at startup ---
	App->gui->CreateIcons();

	std::vector<std::string> filters;
	filters.push_back("fbx");
	filters.push_back("mat");
	filters.push_back("png");
	//filters.push_back("vertex");
	//filters.push_back("VERTEX");

	AssetsFolder = SearchAssets(nullptr, ASSETS_FOLDER, filters);
	DefaultMaterial = (ResourceMaterial*)CreateResource(Resource::ResourceType::MATERIAL, "DefaultMaterial");


	return true;
}

// ------------------------------ IMPORTING --------------------------------------------------------

std::string ModuleResources::DuplicateIntoAssetsFolder(const char* path)
{
	std::string new_path = ASSETS_FOLDER;
	std::string file;

	App->fs->SplitFilePath(path, nullptr, &file, nullptr);
	new_path.append(file);

	if (!App->fs->Exists(new_path.c_str()))
		App->fs->CopyFromOutsideFS(path, new_path.c_str());

	return new_path;
}

// --- Sweep over all files in given directory, if those files pass the given filters, call Import and if it succeeds add them to the given resource folder ---
ResourceFolder* ModuleResources::SearchAssets(ResourceFolder* parent, const char* directory, std::vector<std::string>& filters)
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
		folder->SetParent(parent);

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
				App->fs->NormalizePath(extension);

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
Resource* ModuleResources::ImportAssets(Importer::ImportData& IData)
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Import Switch needs to be updated");

	// --- Only standalone resources go through import here, mesh and material are imported through model's importer ---

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

	case Resource::ResourceType::MATERIAL:
		resource = ImportMaterial(IData);
		break;

	case Resource::ResourceType::SHADER:
		resource = ImportShaderProgram(IData);
		break;

	case Resource::ResourceType::TEXTURE:
		resource = ImportTexture(IData);
		break;

	case Resource::ResourceType::MESH:
		resource = ImportMesh(IData);
		break;

	case Resource::ResourceType::SHADER_OBJECT:
		resource = ImportShaderObject(IData);
		break;

	//case Resource::ResourceType::META:

	//	break;

	case Resource::ResourceType::UNKNOWN:
		break;
	default:
		CONSOLE_LOG("![Warning]: Detected unsupported file type on: %s", IData.path);
		break;
	}

	if (resource)
	{
		if(type != Resource::ResourceType::FOLDER)
		AddResourceToFolder(resource);

		CONSOLE_LOG("Imported successfully: %s", IData.path);
	}
	else
		CONSOLE_LOG("![Warning]: Could not import: %s", IData.path);


	return resource;
}


Resource* ModuleResources::ImportFolder(Importer::ImportData& IData)
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
			IData.path = new_path.append("/").c_str();
			folder = IFolder->Import(IData);
		}
	}

	return folder;
}

Resource* ModuleResources::ImportScene(Importer::ImportData& IData)
{
	ResourceScene* scene = nullptr;

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
	{
		//Loadfromlib
	}

	// --- Else call relevant importer ---
	else
		// Import

	return scene;
}

Resource* ModuleResources::ImportModel(Importer::ImportData& IData)
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
			std::string new_path = DuplicateIntoAssetsFolder(IData.path);
			IData.path = new_path.c_str();
			model = IModel->Import(IData);
		}
	}

	return model;
}

Resource* ModuleResources::ImportMaterial(Importer::ImportData& IData)
{
	ImporterMaterial* IMat = GetImporter<ImporterMaterial>();

	Resource* material = nullptr;

	// --- If the resource is already imported (has meta), load it ---
	if (IsFileImported(IData.path))
		material = IMat->Load(IData.path);

	return material;
}

Resource* ModuleResources::ImportShaderProgram(Importer::ImportData& IData)
{
	ResourceShaderProgram* shader = nullptr;

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
	{
		//Loadfromlib
	}

	// --- Else call relevant importer ---
	else
		// Import

	return shader;
}

Resource* ModuleResources::ImportMesh(Importer::ImportData& IData)
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

Resource* ModuleResources::ImportTexture(Importer::ImportData& IData)
{
	Resource* texture = nullptr;
	ImporterTexture* ITex = GetImporter<ImporterTexture>();
	
	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
		texture = ITex->Load(IData.path);
	

	// --- Else call relevant importer ---
	else
		texture = ITex->Import(IData);

	return texture;
}

Resource* ModuleResources::ImportShaderObject(Importer::ImportData& IData)
{
	ResourceShaderObject* shader_object = nullptr;

	// --- If the resource is already in library, load from there ---
	if (IsFileImported(IData.path))
	{
		//Loadfromlib
	}

	// --- Else call relevant importer ---
	else
		// Import

	return shader_object;
}


// ----------------------------------------------------

// ------------------------------ LOADING --------------------------------------------------------


// ------------------- RESOURCE HANDLING ----------------------------------------------------------

Resource* ModuleResources::GetResource(uint UID)
{
	Resource* resource = nullptr;

	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Get Switch needs to be updated");

	// To clarify: resource = condition ? value to be assigned if true : value to be assigned if false

	resource = folders.find(UID) == folders.end() ? resource : (*folders.find(UID)).second;
	resource = resource ? resource : (scenes.find(UID) == scenes.end() ? resource : (*scenes.find(UID)).second);
	resource = resource ? resource : (models.find(UID) == models.end() ? resource : (*models.find(UID)).second);
	resource = resource ? resource : (materials.find(UID) == materials.end() ? resource : (*materials.find(UID)).second);
	resource = resource ? resource : (shaders.find(UID) == shaders.end() ? resource : (*shaders.find(UID)).second);
	resource = resource ? resource : (meshes.find(UID) == meshes.end() ? resource : (*meshes.find(UID)).second);
	resource = resource ? resource : (textures.find(UID) == textures.end() ? resource : (*textures.find(UID)).second);
	resource = resource ? resource : (shader_objects.find(UID) == shader_objects.end() ? resource : (*shader_objects.find(UID)).second);

	if (resource)
		resource->LoadToMemory();
	else
		CONSOLE_LOG("![Warning]: Could not load: %i", UID);


	return resource;
}

Resource * ModuleResources::CreateResource(Resource::ResourceType type, std::string source_file)
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

	case Resource::ResourceType::SHADER_OBJECT:
		resource = (Resource*)new ResourceShaderObject(App->GetRandom().Int(), source_file);
		shader_objects[resource->GetUID()] = (ResourceShaderObject*)resource;
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

Resource* ModuleResources::CreateResourceGivenUID(Resource::ResourceType type, std::string source_file, uint UID)
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

	case Resource::ResourceType::SHADER_OBJECT:
		resource = (Resource*)new ResourceShaderObject(UID, source_file);
		shader_objects[resource->GetUID()] = (ResourceShaderObject*)resource;
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


Resource::ResourceType ModuleResources::GetResourceTypeFromPath(const char* path)
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Switch needs to be updated");

	std::string extension = "";
	App->fs->SplitFilePath(path, nullptr, nullptr, &extension);
	App->fs->NormalizePath(extension, true);


	Resource::ResourceType type = Resource::ResourceType::UNKNOWN;

	if (extension == "")
		type = Resource::ResourceType::FOLDER;

	else if (extension == "scene")
		type = Resource::ResourceType::SCENE;

	else if (extension == "fbx" || extension == "model")
		type = Resource::ResourceType::MODEL;

	else if (extension == "mat")
		type = Resource::ResourceType::MATERIAL;

	else if (extension == "shader")
		type = Resource::ResourceType::SHADER;

	else if (extension == "dds" || extension == "png" || extension == "jpg")
		type = Resource::ResourceType::TEXTURE;

	else if (extension == "mesh")
		type = Resource::ResourceType::MESH;

	else if (extension == "vertex" || extension == "fragment")
		type = Resource::ResourceType::SHADER_OBJECT;

	else if (extension == "meta")
		type = Resource::ResourceType::META;


	return type;
}

ResourceFolder* ModuleResources::GetAssetsFolder()
{
	return AssetsFolder;
}

uint ModuleResources::GetDefaultMaterialUID()
{
	return DefaultMaterial->GetUID();
}

void ModuleResources::AddResourceToFolder(Resource* resource)
{
	if (resource)
	{
		std::string directory;
		std::string original_file;

		for (std::map<uint, ResourceFolder*>::const_iterator it = folders.begin(); it != folders.end(); ++it)
		{
			// CAREFUL when comparing strings, not putting {} below the if resulted in erroneous behaviour
			directory = App->fs->GetDirectoryFromPath(std::string(resource->GetOriginalFile()));
			directory.pop_back();
			original_file = (*it).second->GetName();
			original_file.pop_back();

			if (directory == original_file)
			{
				(*it).second->AddResource(resource);
			}

		}
	}
}

bool ModuleResources::IsFileImported(const char* file)
{
	bool ret = false;

	if (file)
	{
		std::string path = file;

		path.append(".meta");

		// --- PhysFS only will return true if the file is inside one of the fs predefined folders! 
		//  using that on our advantage to know if a resource is imported or not ---
		ret = App->fs->Exists(path.data());
	}

	return ret;
}

void ModuleResources::ONResourceDestroyed(Resource* resource)
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

	case Resource::ResourceType::MATERIAL:
		materials.erase(resource->GetUID());
		break;

	case Resource::ResourceType::SHADER:
		shaders.erase(resource->GetUID());
		break;

	case Resource::ResourceType::MESH:
		meshes.erase(resource->GetUID());
		break;

	case Resource::ResourceType::TEXTURE:
		textures.erase(resource->GetUID());
		break;

	case Resource::ResourceType::SHADER_OBJECT:
		shader_objects.erase(resource->GetUID());
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

update_status ModuleResources::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 9, "Resource Clean Up needs to be updated");

	// Since std map invalidates iterator on pair delete, a reverse iterator is very useful
	// We eliminate a pair and automatically our iterator points at a new element

	// --- Delete resources ---
	for (std::map<uint, ResourceFolder*>::reverse_iterator it = folders.rbegin(); it != folders.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	folders.clear();

	for (std::map<uint, ResourceScene*>::reverse_iterator it = scenes.rbegin(); it != scenes.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	scenes.clear();

	for (std::map<uint, ResourceModel*>::reverse_iterator it = models.rbegin(); it != models.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	models.clear();

	for (std::map<uint, ResourceMaterial*>::reverse_iterator it = materials.rbegin(); it != materials.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	materials.clear();

	for (std::map<uint, ResourceShader*>::reverse_iterator it = shaders.rbegin(); it != shaders.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	shaders.clear();

	for (std::map<uint, ResourceMesh*>::reverse_iterator it = meshes.rbegin(); it != meshes.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	meshes.clear();

	for (std::map<uint, ResourceTexture*>::reverse_iterator it = textures.rbegin(); it != textures.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	textures.clear();

	for (std::map<uint, ResourceShaderObject*>::reverse_iterator it = shader_objects.rbegin(); it != shader_objects.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
	}

	shader_objects.clear();

	for (std::map<uint, ResourceMeta*>::reverse_iterator it = metas.rbegin(); it != metas.rend();)
	{
		it->second->FreeMemory();
		delete it->second;
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
