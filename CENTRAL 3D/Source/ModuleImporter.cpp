#include "ModuleImporter.h"
#include "Application.h"
#include "ModuleTextures.h"

#include "Importer.h"
#include "ImporterScene.h"
#include "ModuleSceneManager.h"
#include "ModuleFileSystem.h"

#include "GameObject.h"
#include "ComponentMaterial.h"

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
	IScene->Load("Library/Models/BakerHouse.model");
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
			ComponentMaterial* mat = App->scene_manager->GetSelectedGameObject()->GetComponent<ComponentMaterial>(Component::ComponentType::Material);
		
			if (mat->Texture_path == "Default")
			{
				mat = App->scene_manager->CreateEmptyMaterial();

				App->scene_manager->GetSelectedGameObject()->SetMaterial(mat);
			}

			mat->Texture_path = DroppedFile_path.data();

			// --- If there is a material, assign diffuse texture ---
			if (mat)
			{
				App->scene_manager->SetTextureToSelectedGO(App->textures->CreateTextureFromFile(path, mat->Texture_width, mat->Texture_height ));

			}
		}
	}
	else
		ret = false;

	return ret;
}
