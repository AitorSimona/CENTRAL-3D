#include "ModuleResources.h"
#include "ResourceMesh.h"
#include "OpenGL.h"

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

	return true;
}

bool ModuleResources::LoadFile(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array

		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			// Call mesh importer


		}
		aiMesh* mesh = scene->mMeshes[0];

		// --- Vertices ---
		verticesSize = mesh->mNumVertices;
		Vertices = new float3[mesh->mNumVertices];

		for (unsigned i = 0; i < mesh->mNumVertices; ++i)
		{
			Vertices[i] = *((float3*)&mesh->mVertices[i]);
		}

		// --- Indices ---
		IndicesSize = mesh->mNumFaces * 3;
		Indices = new uint[IndicesSize];

		for (unsigned j = 0; j < mesh->mNumFaces; ++j)
		{
			const aiFace& face = mesh->mFaces[j];

			assert(face.mNumIndices == 3); // Only triangles

			Indices[j * 3] = face.mIndices[0];
			Indices[j * 3 + 1] = face.mIndices[1];
			Indices[j * 3 + 2] = face.mIndices[2];
		}

		glGenBuffers(1, (GLuint*)&VerticesID); // create buffer
		glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * verticesSize, Vertices, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer


		glGenBuffers(1, (GLuint*)&IndicesID); // create buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * IndicesSize, Indices, GL_STATIC_DRAW); // send vertices to VRAM
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer

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
	LoadFile("Assets/warrior.fbx");

	return true;
}

void ModuleResources::Draw()
{
	glEnableClientState(GL_VERTEX_ARRAY); // enable client-side capability

	glBindBuffer(GL_ARRAY_BUFFER, VerticesID); // start using created buffer (vertices)
	glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndicesID); // start using created buffer (indices)
	glDrawElements(GL_TRIANGLES, IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)

	glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability
}

