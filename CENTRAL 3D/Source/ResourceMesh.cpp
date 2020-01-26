#include "ResourceMesh.h"
#include "OpenGL.h"

#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh(uint UID, std::string source_file, std::string destination_file) : Resource(Resource::ResourceType::MESH, UID, source_file, destination_file)
{
}

ResourceMesh::~ResourceMesh()
{
	//FreeMemory();
}

void ResourceMesh::CreateAABB()
{
	aabb.SetNegativeInfinity();

	for (uint i = 0; i < VerticesSize; ++i)
	{
		aabb.Enclose(float3(vertices[i].position));
	}
}

void ResourceMesh::LoadInMemory()
{
	CreateVBO();
	CreateEBO();
	CreateVAO();
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
		CONSOLE_LOG("|[error]: Could not create VBO, null vertices");
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
		CONSOLE_LOG("|[error]: Could not create EBO, null indices");
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

	// --- Unbind VAO and VBO ---
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}


