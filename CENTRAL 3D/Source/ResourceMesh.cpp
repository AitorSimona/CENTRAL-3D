#include "ResourceMesh.h"
#include "OpenGL.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"


ResourceMesh::ResourceMesh() : Resource(Resource::ResourceType::mesh)
{
}

ResourceMesh::~ResourceMesh()
{
}

void ResourceMesh::ImportMesh(aiMesh* mesh)
{
	// --- Vertices ---
	this->VerticesSize = mesh->mNumVertices;
	this->Vertices = new Vertex[mesh->mNumVertices];

	for (unsigned j = 0; j < mesh->mNumVertices; ++j)
	{
		this->Vertices[j].position[0] = *((GLfloat*)&mesh->mVertices[j].x);
		this->Vertices[j].position[1] = *((GLfloat*)&mesh->mVertices[j].y);
		this->Vertices[j].position[2] = *((GLfloat*)&mesh->mVertices[j].z);
	}

	// --- Normals ---
	if (mesh->HasNormals())
	{
		for (unsigned j = 0; j < mesh->mNumVertices; ++j)
		{
			this->Vertices[j].normal[0] = *((GLfloat*)&mesh->mNormals[j].x);
			this->Vertices[j].normal[1] = *((GLfloat*)&mesh->mNormals[j].y);
			this->Vertices[j].normal[2] = *((GLfloat*)&mesh->mNormals[j].z);
		}
	}

	// --- Texture Coordinates ---
	for (unsigned j = 0; j < mesh->mNumVertices; ++j)
	{
		if (mesh->HasTextureCoords(j))
		{
			this->Vertices[j].texCoord[0] = *((GLubyte*)&mesh->mTextureCoords[j]->x);
			this->Vertices[j].texCoord[1] = *((GLubyte*)&mesh->mTextureCoords[j]->y);
		}
	}
	

	//glGenTextures(1, (GLuint*)&this->TexID); // create buffer
	//glBindTexture(GL_TEXTURE_2D, this->TexID); // start using created buffer
	////glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1024,1024,GL_RGBA, GL_UNSIGNED_BYTE,GL_RGBA,); // send vertices to VRAM
	//
	//
	//glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer

	// --- Colours ---
	for (unsigned j = 0; j < mesh->mNumVertices; ++j)
	{
		if (mesh->HasVertexColors(j))
		{
			this->Vertices[j].color[0] = *((GLfloat*)&mesh->mColors[j]->r);
			this->Vertices[j].color[1] = *((GLfloat*)&mesh->mColors[j]->g);
			this->Vertices[j].color[2] = *((GLfloat*)&mesh->mColors[j]->b);
			this->Vertices[j].color[3] = *((GLfloat*)&mesh->mColors[j]->a);

		}
	}
	

	// --- Indices ---
	this->IndicesSize = mesh->mNumFaces * 3;
	this->Indices = new uint[this->IndicesSize];

	for (unsigned j = 0; j < mesh->mNumFaces; ++j)
	{
		const aiFace& face = mesh->mFaces[j];

		assert(face.mNumIndices == 3); // Only triangles

		this->Indices[j * 3] = face.mIndices[0];
		this->Indices[j * 3 + 1] = face.mIndices[1];
		this->Indices[j * 3 + 2] = face.mIndices[2];
	}

	GenerateVBO();
	GenerateIBO();
	GenerateVAO();
}

void ResourceMesh::GenerateVBO()
{
	assert(Vertices != nullptr);

	// --- Vertex Buffer Object ---

	// --- Generate a VBO ---
	glGenBuffers(1, &VBO);
	// --- Bind the VBO ---
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VerticesSize, Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenerateIBO()
{
	assert(Indices != nullptr);

	// --- Index Buffer Object ---

	// --- Generate a IBO ---
	glGenBuffers(1, &IBO);

	// --- Bind the IBO ---
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndicesSize, Indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenerateVAO()
{
	// --- Vertex Array Object ---

	// --- Generate a VAO ---
	glGenVertexArrays(1, &VAO);
	// --- Bind the VAO ---
	glBindVertexArray(VAO);

	// --- Bind the VBO we are adding attributes to ---
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// --- Set the vertex attributes ---

	// 1. Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	// 2. Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	glEnableVertexAttribArray(1);

	// 3. Color
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	glEnableVertexAttribArray(2);

	// 4. Texture coords
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Regarding offsetof, stddef macro that returns the offset value in bytes between Vertex class data start
	// and the corresponding attribute (be it position...). For example, in the fourth case, texture coords, this
	// macro returns 10 (add position[3], normal[3] and color[4])
}