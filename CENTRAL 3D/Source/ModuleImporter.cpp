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
	LOG("[Assimp]: %s", msg);
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
	//LoadFromPath("Assets/BakerHouse.fbx");
	//IScene->Load("Library/Models/BakerHouse.model");
	return true;
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
						mat->resource_material->resource_diffuse = (ResourceTexture*)App->resources->CreateResource(Resource::ResourceType::TEXTURE);

						App->scene_manager->GetSelectedGameObject()->SetMaterial(mat);
					}

					mat->resource_material->resource_diffuse->Texture_path = DroppedFile_path.data();

					// --- If there is a material, assign diffuse texture ---
					if (mat)
					{
						App->scene_manager->SetTextureToSelectedGO(App->textures->CreateTextureFromFile(path, mat->resource_material->resource_diffuse->Texture_width, mat->resource_material->resource_diffuse->Texture_height,mat->resource_material->resource_diffuse->GetUID()));

					}
				}
				else
					ret = false;
			}
			else
				ret = false;
		}
	}
	else
		ret = false;

	return ret;
}
