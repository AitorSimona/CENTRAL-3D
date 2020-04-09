#include "ResourceMesh.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"


#include "ImporterMesh.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

using namespace Broken;
ResourceMesh::ResourceMesh(uint UID, const char* source_file) : Resource(Resource::ResourceType::MESH, UID, source_file)
{
	extension = ".mesh";
	resource_file = MESHES_FOLDER + std::to_string(UID) + extension;

	previewTexID = App->gui->meshTexID;
}

ResourceMesh::~ResourceMesh() 
{
	glDeleteTextures(1, (GLuint*)&previewTexID);
}

void ResourceMesh::CreateAABB() 
{
	aabb.SetNegativeInfinity();

	for (uint i = 0; i < VerticesSize; ++i)
		aabb.Enclose(float3(vertices[i].position));
}

bool ResourceMesh::LoadInMemory()
{
	bool ret = true;

	if (App->fs->Exists(resource_file.c_str()))
	{
		// --- Load mesh data ---
		char* buffer = nullptr;
		App->fs->Load(resource_file.c_str(), &buffer);
		char* cursor = buffer;

		// amount of indices / vertices / normals / texture_coords
		uint ranges[3];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		bytes += ranges[0];

		IndicesSize = ranges[1];
		VerticesSize = ranges[2];

		vertices = new Vertex[VerticesSize];
		float* Vertices = new float[VerticesSize * 3];
		float* Normals = new float[VerticesSize * 3];
		unsigned char* Colors = new unsigned char[VerticesSize * 4];
		float* TexCoords = new float[VerticesSize * 2];

		float* Tangents = new float[VerticesSize * 3];
		float* BiTangents = new float[VerticesSize * 3];

		// --- Load indices ---
		cursor += bytes;
		bytes = sizeof(uint) * IndicesSize;
		Indices = new uint[IndicesSize];
		memcpy(Indices, cursor, bytes);

		// --- Load Vertices ---
		cursor += bytes;
		bytes = sizeof(float) * 3 * VerticesSize;
		memcpy(Vertices, cursor, bytes);

		// --- Load Normals ---
		cursor += bytes;
		bytes = sizeof(float) * 3 * VerticesSize;
		memcpy(Normals, cursor, bytes);

		// --- Load Colors ---
		cursor += bytes;
		bytes = sizeof(unsigned char) * 4 * VerticesSize;
		memcpy(Colors, cursor, bytes);

		// --- Load Texture Coords ---
		cursor += bytes;
		bytes = sizeof(float) * 2 * VerticesSize;
		memcpy(TexCoords, cursor, bytes);

		// --- Load Tangents ---
		cursor += bytes;
		bytes = sizeof(float) * 3 * VerticesSize;
		memcpy(Tangents, cursor, bytes);

		// --- Load BiTangents ---
		cursor += bytes;
		bytes = sizeof(float) * 3 * VerticesSize;
		memcpy(BiTangents, cursor, bytes);


		// --- Fill Vertex array ---

		for (uint i = 0; i < VerticesSize; ++i)
		{
			// --- Vertices ---
			vertices[i].position[0] = Vertices[i * 3];
			vertices[i].position[1] = Vertices[(i * 3) + 1];
			vertices[i].position[2] = Vertices[(i * 3) + 2];

			// --- Normals ---
			vertices[i].normal[0] = Normals[i * 3];
			vertices[i].normal[1] = Normals[(i * 3) + 1];
			vertices[i].normal[2] = Normals[(i * 3) + 2];

			// --- Colors ---
			vertices[i].color[0] = Colors[i * 4];
			vertices[i].color[1] = Colors[(i * 4) + 1];
			vertices[i].color[2] = Colors[(i * 4) + 2];
			vertices[i].color[3] = Colors[(i * 4) + 3];

			// --- Texture Coordinates ---
			vertices[i].texCoord[0] = TexCoords[i * 2];
			vertices[i].texCoord[1] = TexCoords[(i * 2) + 1];

			// --- Tangents ---
			vertices[i].tangent[0] = Tangents[i * 3];
			vertices[i].tangent[1] = Tangents[(i * 3) + 1];
			vertices[i].tangent[2] = Tangents[(i * 3) + 2];

			// --- Biitangents ---
			vertices[i].biTangent[0] = BiTangents[i * 3];
			vertices[i].biTangent[1] = BiTangents[(i * 3) + 1];
			vertices[i].biTangent[2] = BiTangents[(i * 3) + 2];
		}

		// --- Delete buffer data ---
		if (buffer)
		{
			delete[] buffer;
			buffer = nullptr;
			cursor = nullptr;
		}

		delete[] Vertices;
		delete[] Normals;
		delete[] Colors;
		delete[] TexCoords;
		delete[] Tangents;
		delete[] BiTangents;
	}

	CreateAABB();
	CreateVBO();
	CreateEBO();
	CreateVAO();

	return ret;
}

void ResourceMesh::FreeMemory()
{
	glDeleteBuffers(1, (GLuint*)&VBO);

	glDeleteBuffers(1, (GLuint*)&EBO);
	glDeleteVertexArrays(1, (GLuint*)&VAO);

	if (vertices)
	{
		delete[] vertices;
		vertices = nullptr;
	}

	if (Indices)
	{
		delete[] Indices;
		Indices = nullptr;
	}

	VBO = 0;
	EBO = 0;
	VAO = 0;
}

void ResourceMesh::CreateVBO()
{
	if (vertices != nullptr)
	{
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VerticesSize, vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Could not create VBO, null vertices");
}

void ResourceMesh::CreateEBO()
{
	if (Indices != nullptr)
	{
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * IndicesSize, Indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
		ENGINE_CONSOLE_LOG("|[error]: Could not create EBO, null indices");
}

void ResourceMesh::CreateVAO()
{
	// --- Create a Vertex Array Object ---
	glGenVertexArrays(1, &VAO);
	// --- Bind it ---
	glBindVertexArray(VAO);

	// Bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// --- Set all vertex attribute pointers ---

	// --- Vertex Position ---
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	// --- Vertex Normal ---
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	// --- Vertex Color ---
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	glEnableVertexAttribArray(2);

	// --- Vertex Texture coordinates ---
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	glEnableVertexAttribArray(3);

	// --- Vertex Tangents ---
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, tangent)));
	glEnableVertexAttribArray(4);

	// --- Vertex Bitangents ---
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, biTangent)));
	glEnableVertexAttribArray(5);

	// --- Unbind VAO and VBO ---
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::OnOverwrite()
{
	// Since mesh is not a standalone resource (which means it is always owned by a model) the model is in charge
	// of overwriting it (see ResourceModel OnOverwrite for details)
	NotifyUsers(ResourceNotificationType::Overwrite);
}

void ResourceMesh::OnDelete() 
{
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();

	if(App->fs->Exists(resource_file.c_str()))
	App->fs->Remove(resource_file.c_str());
	App->fs->Remove(previewTexPath.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}

void ResourceMesh::Repath() 
{
	ImporterMesh* IMesh = App->resources->GetImporter<ImporterMesh>();
	IMesh->Save(this);
}


