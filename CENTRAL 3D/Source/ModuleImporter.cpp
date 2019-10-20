#include "ModuleImporter.h"
#include "Application.h"
#include "ModuleTextures.h"

#include "Importer.h"
#include "ImporterScene.h"
#include "ModuleSceneManager.h"

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
	LoadFromPath("Assets/BakerHouse.fbx");

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

bool ModuleImporter::LoadFromPath(const char* path)
{
	bool ret = true;

	// MYTODO: Any load operation should go through this function and then decide which importer to call based on file extension
	// MYTODO: Could JSONLoader be a new importer?

	if (path)
	{
		std::string DroppedFile_path = path;

		// If it is a 3D Model ...
		if (DroppedFile_path.find(".fbx") != std::string::npos || DroppedFile_path.find(".FBX") != std::string::npos)
		{
			ImportData data;
			ret = IScene->Import(*path, data);
		}
		// If it is a json file ...
		else if (DroppedFile_path.find(".json") != std::string::npos || DroppedFile_path.find(".JSON") != std::string::npos)
		{

		}
		// If it is an image file file ...
		else if (DroppedFile_path.find(".dds") != std::string::npos || DroppedFile_path.find(".png") != std::string::npos)
		{
			// MYTODO: We are not checking if the texture was already loaded, duplicating data
			App->scene_manager->SetTextureToSelectedGO(App->textures->CreateTextureFromFile(path));
		}
	}
	else
		ret = false;

	return ret;
}
