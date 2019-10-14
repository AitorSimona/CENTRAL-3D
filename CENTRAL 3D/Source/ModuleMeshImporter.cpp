#include "ModuleMeshImporter.h"
#include "ModuleTextures.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "ComponentRenderer.h"

#include "GameObject.h"
#include "OpenGL.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"

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

ModuleMeshImporter::ModuleMeshImporter(bool start_enabled) : Module(start_enabled)
{
}

ModuleMeshImporter::~ModuleMeshImporter()
{
}

bool ModuleMeshImporter::Init(json file)
{
	// --- Stream LOG messages to MyAssimpCallback, that sends them to console ---
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	aiAttachLogStream(&stream);

	return true;
}

bool ModuleMeshImporter::Start()
{
	LoadFBX("Assets/BakerHouse.fbx");

	return true;
}


bool ModuleMeshImporter::CleanUp()
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

bool ModuleMeshImporter::LoadFBX(const char* path)
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
		LOG("|[error]: Error loading scene %s", path);


	return true;
}


void ModuleMeshImporter::Draw() const
{
	// --- Activate wireframe mode ---
	if (App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// --- Draw Meshes ---
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

// MYTODO: Change Name of this function
uint ModuleMeshImporter::GetNumMeshes() const
{
	return game_objects.size();
}


//void ModuleMeshImporter::DrawMesh(ComponentMesh * mesh) const
//{
//	// --- Draw Texture ---
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY); // enable gl capability
//	glEnable(GL_TEXTURE_2D); // enable gl capability
//
//	// --- If the mesh has a material associated, get it ---
//	ComponentMaterial* mat = (ComponentMaterial*)mesh->GetContainerGameObject()->GetComponent(Component::ComponentType::Material);
//
//	if (mat)
//	{
//		glBindTexture(GL_TEXTURE_2D, mat->TextureID); // start using texture
//		glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should set which one is active 
//		glBindBuffer(GL_ARRAY_BUFFER, mesh->TextureCoordsID); // start using created buffer (tex coords)
//		glTexCoordPointer(2, GL_FLOAT, 0, NULL); // Specify type of data format
//	}
//
//	// --- Draw mesh ---
//	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability
//	glBindBuffer(GL_ARRAY_BUFFER, mesh->VerticesID); // start using created buffer (vertices)
//	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IndicesID); // start using created buffer (indices)
//	glDrawElements(GL_TRIANGLES, mesh->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data
//
//	// ----        ----
//
//	// --- Unbind buffers, Disable capabilities ---
//
//	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)
//	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY); // disable client-side capability
//	glActiveTexture(GL_TEXTURE0); // In case we had multitexturing, we should reset active texture
//	glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer (texture)
//}
//
//void ModuleMeshImporter::DrawNormals(const ComponentMesh * mesh) const
//{
//	// --- Draw Mesh Normals ---
//
//	if (mesh->Normals)
//	{
//		// --- Draw Vertex Normals ---
//
//		glBegin(GL_LINES);
//		glLineWidth(1.0f);
//
//		glColor4f(0.0f, 0.5f, 0.5f, 1.0f);
//
//	/*	for (uint j = 0; j < mesh->VerticesSize; ++j)
//		{
//			glVertex3f(mesh->Vertices[j].x, mesh->Vertices[j].y, mesh->Vertices[j].z);
//			glVertex3f(mesh->Vertices[j].x + mesh->Normals[j].x*NORMAL_LENGTH, mesh->Vertices[j].y + mesh->Normals[j].y*NORMAL_LENGTH, mesh->Vertices[j].z + mesh->Normals[j].z*NORMAL_LENGTH);
//		}
//*/
//		// --- Draw Face Normals 
//
//		Triangle face;
//
//		for (uint j = 0; j < mesh->VerticesSize / 3; ++j)
//		{
//			face.a = mesh->Vertices[(j * 3)];
//			face.b = mesh->Vertices[(j * 3) + 1];
//			face.c = mesh->Vertices[(j * 3) + 2];
//
//			float3 face_center = face.Centroid();
//			//float3 face_normal = face.NormalCW();
//			
//			float3 face_normal = Cross(face.a - face.b, face.c - face.b);
//
//			face_normal.Normalize();
//
//			glVertex3f(face_center.x, face_center.y, face_center.z);
//			glVertex3f(face_center.x + face_normal.x*NORMAL_LENGTH, face_center.y + face_normal.y*NORMAL_LENGTH, face_center.z + face_normal.z*NORMAL_LENGTH);
//		}
//
//		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
//		glEnd();
//
//	}
//
//}
//
