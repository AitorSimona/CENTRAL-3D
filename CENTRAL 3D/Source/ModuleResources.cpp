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
		glDeleteBuffers(1, (GLuint*)&meshes[i]->VBO);
		glDeleteBuffers(1, (GLuint*)&meshes[i]->IBO);

		RELEASE_ARRAY(meshes[i]->Vertices);
		RELEASE_ARRAY(meshes[i]->Indices);

		delete meshes[i];
	}

	return true;
}

bool ModuleResources::LoadFBX(const char* path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{

		// Use scene->mNumMeshes to iterate on scene->mMeshes array

		for (uint i = 0; i < scene->mNumMeshes; ++i)
		{
			// --- Create new Resource mesh to store current scene mesh data ---
			ResourceMesh* new_mesh = new ResourceMesh;
			meshes.push_back(new_mesh);

			// --- Get Scene mesh number i ---
			aiMesh* mesh = scene->mMeshes[i];

			// --- Import mesh data (fill new_mesh)---
			new_mesh->ImportMesh(mesh);
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

		//glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->VBO); // start using created buffer (vertices)
		//glVertexPointer(3, GL_FLOAT, 0, NULL); // Use selected buffer as vertices 

		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->IBO); // start using created buffer (indices)
		//glDrawElements(GL_TRIANGLES, meshes[i]->IndicesSize, GL_UNSIGNED_INT, NULL); // render primitives from array data

		//glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer (vertices)
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer (indices)

		// --- SHOULD CREATE DRAW FUNCTION IN RENDERER TO DRAW MESHES ---
		glBindVertexArray(meshes[i]->VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->IBO);

		glDrawElements(GL_TRIANGLES, meshes[i]->IndicesSize, GL_UNSIGNED_INT, NULL);

		/*glBindBuffer(GL_ARRAY_BUFFER, 0);*/
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDisableClientState(GL_VERTEX_ARRAY); // disable client-side capability


		//if (meshes[i]->TexCoords)
		//{
		//	glEnable(GL_TEXTURE_2D);

		//	//glEnableClientState(GL_TEXTURE_2D);

		//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->TexID); // start using created buffer (indices)
		//	glDrawArrays(GL_TEXTURE_2D, 0, meshes[i]->TexCoordsSize);

		//	//glDisableClientState(GL_TEXTURE_2D);

		//	glDisable(GL_TEXTURE_2D);

		//}

		//if (meshes[i]->Normals)
		//{
		//	glBegin(GL_LINES);
		//	glLineWidth(1.0f);
		//	uint Normal_length = 1;

		//	glColor4f(0.0f, 0.5f, 0.5f, 1.0f);

		//	for (uint j = 0; j < meshes[i]->VerticesSize; ++j)
		//	{
		//		glVertex3f(meshes[i]->Vertices[j].x, meshes[i]->Vertices[j].y, meshes[i]->Vertices[j].z);
		//		glVertex3f(meshes[i]->Vertices[j].x + meshes[i]->Normals[j].x*Normal_length, meshes[i]->Vertices[j].y + meshes[i]->Normals[j].y*Normal_length, meshes[i]->Vertices[j].z + meshes[i]->Normals[j].z*Normal_length);
		//	}

		//	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

		//	glEnd();

		//}

	}

	// --- DeActivate wireframe mode ---
	if (App->renderer3D->wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

