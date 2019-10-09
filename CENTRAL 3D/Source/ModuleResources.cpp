#include "ModuleResources.h"
#include "ResourceMesh.h"
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
	LOG("[Assimp]: %s", msg);
}

bool ModuleResources::CleanUp()
{
	// detach log stream
	aiDetachAllLogStreams();


	for (uint i = 0; i < meshes.size(); ++i)
	{
		glDeleteBuffers(1, (GLuint*)&meshes[i]->VerticesID);
		glDeleteBuffers(1, (GLuint*)&meshes[i]->IndicesID);

		RELEASE_ARRAY(meshes[i]->Vertices);
		RELEASE_ARRAY(meshes[i]->Indices);

		delete meshes[i];
	}

	return true;
}

bool ModuleResources::LoadFile(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{

		//meshes.reserve(scene->mNumMeshes);

		// Use scene->mNumMeshes to iterate on scene->mMeshes array

		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			ResourceMesh* new_mesh = new ResourceMesh;
			meshes.push_back(new_mesh);

			// Call mesh importer
			aiMesh* mesh = scene->mMeshes[i];

			// --- Vertices ---
			meshes[i]->verticesSize = mesh->mNumVertices;
			meshes[i]->Vertices = new float3[mesh->mNumVertices];

			for (unsigned j = 0; j < mesh->mNumVertices; ++j)
			{
				meshes[i]->Vertices[j] = *((float3*)&mesh->mVertices[j]);
			}

			glGenBuffers(1, (GLuint*)&meshes[i]->VerticesID); // create buffer
			glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->VerticesID); // start using created buffer
			glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * meshes[i]->verticesSize, meshes[i]->Vertices, GL_STATIC_DRAW); // send vertices to VRAM
			glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer


			// --- Indices ---
			meshes[i]->IndicesSize = mesh->mNumFaces * 3;
			meshes[i]->Indices = new uint[meshes[i]->IndicesSize];

			for (unsigned j = 0; j < mesh->mNumFaces; ++j)
			{
				const aiFace& face = mesh->mFaces[j];

				assert(face.mNumIndices == 3); // Only triangles

				meshes[i]->Indices[j * 3] = face.mIndices[0];
				meshes[i]->Indices[j * 3 + 1] = face.mIndices[1];
				meshes[i]->Indices[j * 3 + 2] = face.mIndices[2];
			}


			glGenBuffers(1, (GLuint*)&meshes[i]->IndicesID); // create buffer
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->IndicesID); // start using created buffer
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * meshes[i]->IndicesSize, meshes[i]->Indices, GL_STATIC_DRAW); // send vertices to VRAM
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer


		}
		aiReleaseImport(scene);


	}
	else
		LOG("|[error]: Error loading scene %s", path);


	return true;
}

ModuleResources::ModuleResources(Application * app, bool start_enabled) : Module(app,start_enabled)
{
}

ModuleResources::~ModuleResources()
{
}

bool ModuleResources::Init(json file)
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream.callback = MyAssimpCallback;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);




	return true;
}

bool ModuleResources::Start()
{
	//LoadFile("Assets/warrior.fbx");

	return true;
}

void ModuleResources::Draw()
{
	// --- Activate wireframe mode ---

	if (App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	for (uint i = 0; i < meshes.size(); ++i)
	{
		glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability

		glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->VerticesID); // start using created buffer (vertices)
		glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->IndicesID); // start using created buffer (indices)
		glDrawElements(GL_TRIANGLES, meshes[i]->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

		glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)

		glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
	}

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

