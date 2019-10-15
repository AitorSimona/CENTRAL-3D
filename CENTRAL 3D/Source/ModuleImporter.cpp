#include "ModuleImporter.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentRenderer.h"

#include "GameObject.h"
#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"


#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

#include "mmgr/mmgr.h"

void MyAssimpCallback(const char * msg, char * userData)
{
	// --- Get Assimp LOGS and print them to console ---
	LOG("[Assimp]: %s", msg);
}

ModuleImporter::ModuleImporter(bool start_enabled) : Module(start_enabled)
{
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
	LoadFBX("Assets/BakerHouse.fbx");

	return true;
}


bool ModuleImporter::CleanUp()
{
	// --- Detach assimp log stream ---
	aiDetachAllLogStreams();

	// -- Release all buffer data and own stored data ---
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		if(game_objects[i])
		delete game_objects[i];
	}

	for (uint i = 0; i < Materials.size(); ++i)
	{
		if (Materials[i])
		delete Materials[i];
	}


	return true;
}

bool ModuleImporter::LoadFBX(const char* path)
{
	// --- Import scene from path ---
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);


	if (scene != nullptr && scene->HasMeshes())
	{

		// --- Create new Component Material to store scene's, meshes will use this for now since we do not want to create a material for every mesh if not needed ---

		ComponentMaterial* Material = new ComponentMaterial(Component::ComponentType::Material);
		Material->CreateFromScene(scene, path);

		if(Material)
		Materials.push_back(Material);

		// --- Use scene->mNumMeshes to iterate on scene->mMeshes array ---

		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			// --- Create Game Object and add it to list to keep track of it ---
			GameObject* new_object = new GameObject("GO"); // MYTODO: We should create name like GameObject, GameObject1 ...
			game_objects.push_back(new_object);

			// --- Get Scene mesh number i ---
			aiMesh* mesh = scene->mMeshes[i];

			if (mesh)
			{
				// --- Create new Component Mesh to store current scene mesh data ---
				ComponentMesh* new_mesh = (ComponentMesh*)new_object->AddComponent(Component::ComponentType::Mesh);

				// --- Import mesh data (fill new_mesh)---
				new_mesh->ImportMesh(mesh);

				// --- Create new Component Renderer to draw mesh ---
				ComponentRenderer* Renderer = (ComponentRenderer*)new_object->AddComponent(Component::ComponentType::Renderer);


			}
			if (Material)
			{
				// --- Set Object's Material ---
				new_object->SetMaterial(Material);
			}
		}

		// --- Free scene ---
		aiReleaseImport(scene);

	}
	else
		LOG("|[error]: Error loading FBX %s", path);


	return true;
}


void ModuleImporter::Draw() const
{
	// --- Activate wireframe mode ---
	if (App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// --- Draw Game Object Meshes ---
	for (uint i = 0; i < game_objects.size(); ++i)
	{
		ComponentRenderer* Renderer = (ComponentRenderer*) game_objects[i]->GetComponent(Component::ComponentType::Renderer);

		if (Renderer)
		{
			Renderer->Draw();
		}
		else
		{
			//LOG("|[error]: Could not find Renderer component in current game object");
		}
	}

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

uint ModuleImporter::GetNumGameObjects() const
{
	return game_objects.size();
}
