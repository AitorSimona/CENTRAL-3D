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
	
	//glGenBuffers(1, (GLuint*)&this->VerticesID); // create buffer
	//glBindBuffer(GL_ARRAY_BUFFER, this->VerticesID); // start using created buffer
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * this->VerticesSize, this->Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	//// --- Normals ---
	//if (mesh->HasNormals())
	//{
	//	NormalsSize = mesh->mNumVertices;
	//	Normals = new float3[NormalsSize];
	//	memcpy(Normals, mesh->mNormals, sizeof(float3)*NormalsSize);
	//}

	//// --- Texture Coordinates ---

	//if (mesh->HasTextureCoords(0))
	//{
	//	uint TexCoordsSize = mesh->mNumVertices;
	//	float2 TexCoords[TexCoordsSize] /*= new float2[TexCoordsSize]*/;

	//	for (unsigned i = 0; i < TexCoordsSize; ++i)
	//	{
	//		TexCoords[i] = float2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	//	}
	//}

	//glGenTextures(1, (GLuint*)&this->TexID); // create buffer
	//glBindTexture(GL_TEXTURE_2D, this->TexID); // start using created buffer
	////glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,1024,1024,GL_RGBA, GL_UNSIGNED_BYTE,GL_RGBA,); // send vertices to VRAM
	//
	//
	//glBindTexture(GL_TEXTURE_2D, 0); // Stop using buffer

	//// --- Colours ---

	//if (mesh->HasVertexColors(0))
	//{
	//	ColoursSize = VerticesSize;
	//	Colours = new unsigned char[ColoursSize * 4];
	//	memcpy(Colours, mesh->mColors, sizeof(unsigned char)*ColoursSize*4);
	//}

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

	//glGenBuffers(1, (GLuint*)&this->IndicesID); // create buffer
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndicesID); // start using created buffer
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->IndicesSize, this->Indices, GL_STATIC_DRAW); // send vertices to VRAM
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer
}

void ResourceMesh::GenerateVBO()
{
	assert(Vertices != nullptr);

	// Vertex Buffer Object

	// Generate a VBO
	glGenBuffers(1, &VBO);
	// Bind the VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * VerticesSize, Vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenerateIBO()
{
	assert(Indices != nullptr);

	// Index Buffer Object

	// Generate a IBO
	glGenBuffers(1, &IBO);
	// Bind the IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * IndicesSize, Indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ResourceMesh::GenerateVAO()
{
	// Vertex Array Object

	// Generate a VAO
	glGenVertexArrays(1, &VAO);
	// Bind the VAO
	glBindVertexArray(VAO);

	// Bind the VBO 
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	// Set the vertex attributes pointers
	// 1. Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, position)));
	glEnableVertexAttribArray(0);

	//// 2. Normal
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
	//glEnableVertexAttribArray(1);

	//// 3. Color
	//glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)(offsetof(Vertex, color)));
	//glEnableVertexAttribArray(2);

	//// 4. Tex coords
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, texCoord)));
	//glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}