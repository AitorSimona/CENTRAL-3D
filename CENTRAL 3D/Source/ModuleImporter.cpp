#include "ModuleImporter.h"
#include "Application.h"
#include "ModuleTextures.h"

#include "Importer.h"
#include "ImporterScene.h"
#include "ModuleSceneManager.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "ComponentMaterial.h"

#include "ModuleResources.h"
#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Assimp/include/cimport.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"

void MyAssimpCallback(const char * msg, char * userData)
{
	// --- Get Assimp LOGS and print them to console ---
	CONSOLE_LOG("[Assimp]: %s", msg);
}

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
	IScene = new ImporterScene;
}

ModuleImporter::~ModuleImporter()
{
}

bool ModuleImporter::Init(json file)
{
	// --- Stream LOG messages to MyAssimpCallback, that sends them to console ---
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleImporter::Start()
{
	std::vector<std::string> filters;
	filters.push_back("fbx");
	filters.push_back("FBX");

	ImportAssets(ASSETS_FOLDER, filters);

	return true;
}

void ModuleImporter::ImportAssets(const char * directory, std::vector<std::string>& filters)
{
		std::vector<std::string> files;
		std::vector<std::string> dirs;

		std::string dir((directory) ? directory : "");
		dir += "/";

		App->fs->DiscoverFiles(dir.c_str(), files, dirs);

		for (std::vector<std::string>::const_iterator it = dirs.begin(); it != dirs.end(); ++it)
		{
				ImportAssets((dir + (*it)).c_str(), filters);
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

			if (pass_filter)
			{
				std::string path = directory;
				path.append((*it).data());
				LoadFromPath(path.data());
			}
		}
}



bool ModuleImporter::CleanUp()
{
	// --- Detach assimp log stream ---
	aiDetachAllLogStreams();

	// --- Destroy all Importers ---
	if (IScene)
	{
		delete IScene;
		IScene = nullptr;
	}

	return true;
}

ImporterScene * ModuleImporter::GetImporterScene() const
{
	return IScene;
}

bool ModuleImporter::LoadFromPath(const char* path) const
{
	bool ret = true;

	// MYTODO: Could JSONLoader be a new importer?

	if (path)
	{
		std::string DroppedFile_path = path;
		App->fs->NormalizePath(DroppedFile_path);

		// If it is a 3D Model ...
		if (DroppedFile_path.find(".fbx") != std::string::npos || DroppedFile_path.find(".FBX") != std::string::npos)
		{
			ImportData data;
			ret = IScene->Import(DroppedFile_path.data(), data);
		}
		// If it is a json file ...
		else if (DroppedFile_path.find(".json") != std::string::npos || DroppedFile_path.find(".JSON") != std::string::npos)
		{

		}
		// If it is an image file file ...
		else if (DroppedFile_path.find(".dds") != std::string::npos || DroppedFile_path.find(".png") != std::string::npos)
		{
			// MYTODO: We are not checking if the texture was already loaded, duplicating data

			// --- Get Selected Game Object's Material ---
			GameObject* Selected = App->scene_manager->GetSelectedGameObject();
			ComponentMaterial* mat = nullptr;

			if (Selected)
			{
				mat = Selected->GetComponent<ComponentMaterial>(Component::ComponentType::Material);

				if (mat)
				{
					if (mat->resource_material->resource_diffuse->Texture_path == "Default")
					{
						mat = App->scene_manager->CreateEmptyMaterial();
						mat->resource_material = (ResourceMaterial*)App->resources->CreateResource(Resource::ResourceType::MATERIAL);
					}

					std::string destination = ASSETS_FOLDER;
					std::string filename;
					App->fs->SplitFilePath(path, nullptr, &filename, nullptr);
					destination.append(filename);

					if (!App->fs->Exists(destination.data()))
						App->fs->CopyFromOutsideFS(path, destination.data());

					ResourceTexture* tex = nullptr;

					// --- Look for meta, if found load image from library ---
					if (App->resources->IsFileImported(destination.data()))
					{
						uint uid = App->resources->GetUIDFromMeta(destination.data());

						std::string lib_Tex = TEXTURES_FOLDER;
						lib_Tex.append(std::to_string(uid));
						lib_Tex.append(".dds");

						tex = (ResourceTexture*)App->resources->GetResource(lib_Tex.data());

					}
					else
					tex = (ResourceTexture*)App->resources->GetResource(destination.data());

					if (tex)
					{
						if (mat->resource_material->resource_diffuse)
						{
							mat->resource_material->resource_diffuse->instances--;
							// MYTODO: UNLOAD RESOURCE FROM MEMORY
						}

						mat->resource_material->resource_diffuse = tex;
					}
					else
					{
						mat->resource_material->resource_diffuse = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);
						App->scene_manager->SetTextureToSelectedGO(App->textures->CreateTextureFromFile(destination.data(), mat->resource_material->resource_diffuse->Texture_width, mat->resource_material->resource_diffuse->Texture_height, mat->resource_material->resource_diffuse->GetUID()));
						
						std::string lib_Tex = TEXTURES_FOLDER;
						lib_Tex.append(std::to_string(mat->resource_material->resource_diffuse->GetUID()));
						lib_Tex.append(".dds");

						mat->resource_material->resource_diffuse->Texture_path = lib_Tex.data();
						mat->resource_material->resource_diffuse->SetOriginalFilename(lib_Tex.data());
					}

					App->scene_manager->GetSelectedGameObject()->SetMaterial(mat);

				}

			}
			else
				ret = false;
		}
	}
	else
		ret = false;

	return ret;
}
