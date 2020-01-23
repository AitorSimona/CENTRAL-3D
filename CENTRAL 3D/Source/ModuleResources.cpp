#include "ModuleResources.h"
#include "Application.h"
#include "ModuleFileSystem.h"


#include "ImporterScene.h"
#include "ImporterMesh.h"

#include "ResourceModel.h"
#include "ResourceScene.h"
#include "ResourceShaderProgram.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "ResourceShaderObject.h"
#include "ResourceShader.h"

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

	return true;
}

bool ModuleResources::Start()
{
	// --- Import all resources in Assets at startup ---

	//std::vector<std::string> filters;
	//filters.push_back("vertex");
	//filters.push_back("VERTEX");

	//ImportAssets(ASSETS_FOLDER, filters);

	return true;
}


// ------------------------------ IMPORTING --------------------------------------------------------

// --- Sweep over all files in given directory, if those files pass the given filters, call Import ---
void ModuleResources::SearchAssets(const char* directory, std::vector<std::string>& filters)
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;

	std::string dir((directory) ? directory : "");
	dir += "/";

	App->fs->DiscoverFiles(dir.c_str(), files, dirs);

	for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
	{
		SearchAssets((dir + (*it) + "/").c_str(), filters);
	}

	std::sort(files.begin(), files.end());

	for (std::vector<std::string>::const_iterator it = files.begin(); it != files.end(); ++it)
	{
		const std::string& str = *it;

		bool pass_filter = false;

		if (filters.size() > 0)
		{
			for (uint i = 0; i < filters.size(); ++i)
			{
				if (str.substr(str.find_last_of(".") + 1) == filters[i])
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
			ImportAssets(path.data());
		}
	}
}

// --- Identify resource by file extension, call relevant importer, prepare everything for its use ---
void ModuleResources::ImportAssets(const char* path)
{
	// --- Identify resource type by file extension ---
	Resource::ResourceType type = GetResourceTypeFromPath(path);
	Resource* resource = nullptr;

	// --- Call relevant function depending on resource type ---

	switch (type)
	{
	case Resource::ResourceType::MESH:
		ImportMesh(path);
		break;
	case Resource::ResourceType::TEXTURE:
		ImportTexture(path);
		break;
	case Resource::ResourceType::MATERIAL:
		ImportMaterial(path);
		break;
	case Resource::ResourceType::META:

		break;
	case Resource::ResourceType::SCENE:
		ImportScene(path);
		break;
	case Resource::ResourceType::MODEL:
		ImportModel(path);
		break;
	case Resource::ResourceType::SHADER:
		ImportShaderProgram(path);
		break;
	case Resource::ResourceType::SHADER_OBJECT:
		ImportShaderObject(path);
		break;
	case Resource::ResourceType::UNKNOWN:
		break;
	default:
		CONSOLE_LOG("![Warning]: Detected unsupported file type on: %s", path);
		break;
	}

	if (resource)
	{
		CONSOLE_LOG("Imported successfully: %s", path);
	}
	else
		CONSOLE_LOG("![Warning]: Could not import: %s", path);


}

Resource* ModuleResources::ImportScene(const char* path)
{
	ResourceScene* scene = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return scene;
}

Resource* ModuleResources::ImportModel(const char* path)
{
	ResourceModel* model = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return model;
}

Resource* ModuleResources::ImportMaterial(const char* path)
{
	ResourceMaterial* material = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return material;
}

Resource* ModuleResources::ImportShaderProgram(const char* path)
{
	ResourceShaderProgram* shader = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return shader;
}

Resource* ModuleResources::ImportMesh(const char* path)
{
	ResourceMesh* mesh = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return mesh;
}

Resource* ModuleResources::ImportTexture(const char* path)
{
	ResourceTexture* texture = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return texture;
}

Resource* ModuleResources::ImportShaderObject(const char* path)
{
	ResourceShaderObject* shader_object = nullptr;

	// --- If the resource is already in library, load from there ---


	// --- Else call relevant importer ---

	return shader_object;
}

Resource::ResourceType ModuleResources::GetResourceTypeFromPath(const char* path)
{
	static_assert(static_cast<int>(Resource::ResourceType::UNKNOWN) == 8, "Resource Creation Switch needs to be updated");

	std::string extension = "";
	App->fs->SplitFilePath(path, nullptr, nullptr, &extension);
	App->fs->NormalizePath(extension,true);


	Resource::ResourceType type = Resource::ResourceType::UNKNOWN;

	if (extension == ".mesh")
		type = Resource::ResourceType::MESH;

	else if (extension == ".dds" || extension == ".png" || extension == ".jpg")
		type = Resource::ResourceType::TEXTURE;

	else if (extension == ".vertex" || extension == ".fragment")
		type = Resource::ResourceType::SHADER_OBJECT;

	else if (extension == ".scene")
		type = Resource::ResourceType::SCENE;

	else if (extension == ".model" || extension == ".fbx" || extension == ".obj")
		type = Resource::ResourceType::MODEL;

	else if (extension == ".shader")
		type = Resource::ResourceType::SHADER;

	else if (extension == ".mat")
		type = Resource::ResourceType::MATERIAL;

	else if (extension == ".meta")
		type = Resource::ResourceType::META;

	return type;
}


// ----------------------------------------------------

update_status ModuleResources::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResources::CleanUp()
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end();)
	{
		it->second->FreeMemory();
		delete it->second;
		it = resources.erase(it);
	}

	// --- Detach assimp log stream ---
	aiDetachAllLogStreams();

	return true;
}


//void ModuleResources::CreateMetaFromUID(uint UID,const char* filename)
//{
//	ResourceMeta meta;
//
//	json jsonmeta;
//	std::string jsondata;
//	std::string meta_path;
//	char* meta_buffer = nullptr;
//
//	// --- Create Meta ---
//	jsonmeta["UID"] = std::to_string(UID);
//	jsonmeta["DATE"] = std::to_string(App->fs->GetLastModificationTime(filename));
//	jsondata = App->GetJLoader()->Serialize(jsonmeta);
//	meta_buffer = (char*)jsondata.data();
//
//
//	meta_path = filename;
//	meta_path.append(".meta");
//
//	LoadedResources[UID] = meta;
//	App->fs->Save(meta_path.data(), meta_buffer, jsondata.length());
//}
//
//bool ModuleResources::IsFileImported(const char * file)
//{
//	bool ret = false;
//
//	std::string path = file;
//
//	path.append(".meta");
//
//	ret = App->fs->Exists(path.data());
//
//	return ret;
//}
//
//Resource * ModuleResources::GetResource(const char * original_file)
//{
//	// --- If resource is loaded into memory, return pointer to it, else load it ---
//
//	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
//	{
//		std::string tmp = it->second->GetOriginalFile();
//		if (tmp.compare(original_file) == 0)
//			return it->second;
//	}
//
//	return nullptr;
//}
//
//
//uint ModuleResources::GetUIDFromMeta(const char * file)
//{
//	std::string path = file;
//	path.append(".meta");
//	uint UID = 0;
//
//	if (App->fs->Exists(path.data()))
//	{
//		json file = App->GetJLoader()->Load(path.data());
//		std::string uid = file["UID"];
//		UID = std::stoi(uid);
//	}
//
//	return UID;
//}
//
//uint ModuleResources::GetModDateFromMeta(const char * file)
//{
//	std::string path = file;
//	path.append(".meta");
//	uint DATE = 0;
//
//	if (App->fs->Exists(path.data()))
//	{
//		json file = App->GetJLoader()->Load(path.data());
//		std::string date = file["DATE"];
//		DATE = std::stoi(date);
//	}
//
//	return DATE;
//}
//
//std::map<std::string, ResourceShader*>* ModuleResources::GetShaders()
//{
//	return &shaders;
//}
//
//void ModuleResources::SaveAllShaders()
//{
//	for (std::map<std::string, ResourceShader*>::iterator it = shaders.begin(); it != shaders.end(); ++it)
//	{
//		if(it->first != "Standard" && it->first != "LinePoint" && it->first != "ZDrawer" && it->first != "OutlineShader")
//		it->second->Save();
//	}
//}
//
//Resource * ModuleResources::CreateResource(Resource::ResourceType type)
//{
//	Resource* resource = nullptr;
//
//	switch (type)
//	{
//	case Resource::ResourceType::MESH:
//		resource = (Resource*)new ResourceMesh;
//		break;
//
//	case Resource::ResourceType::TEXTURE:
//		resource = (Resource*)new ResourceTexture;
//		break;
//
//	case Resource::ResourceType::MATERIAL:
//		resource = (Resource*)new ResourceMaterial;
//		break;
//
//	case Resource::ResourceType::SHADER:
//		resource = (Resource*)new ResourceShader;
//		break;
//	}
//
//	if (resource)
//		resources[resource->GetUID()] = resource;
//
//	return resource;
//}
//
//void ModuleResources::AddResource(Resource* res)
//{
//	if (res)
//		resources[res->GetUID()] = res;
//}
//
//void ModuleResources::AddShader(ResourceShader * shader)
//{
//	if(shader)
//		shaders[shader->name] = shader;
//}
//
//
//
