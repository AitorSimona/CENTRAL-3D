#include "ComponentMesh.h"
#include "OpenGL.h"
#include "GameObject.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh(GameObject* ContainerGO) : Component(ContainerGO,Component::ComponentType::Mesh)
{
}

ComponentMesh::~ComponentMesh()
{
	glDeleteBuffers(1, (GLuint*)&VerticesID);
	glDeleteBuffers(1, (GLuint*)&IndicesID);
	glDeleteBuffers(1, (GLuint*)&TextureCoordsID);

	// MYTODO: Should not delete the texture if it is shared with another mesh?
	glDeleteBuffers(1, (GLuint*)&TextureID);

	if (Vertices)
	{
		delete[] Vertices;
		Vertices = nullptr;
	}
	if (Indices)
	{
		delete[] Indices;
		Indices = nullptr;
	}
	if (Normals)
	{
		delete[] Normals;
		Normals = nullptr;
	}
	if (TexCoords)
	{
		delete[] TexCoords;
		TexCoords = nullptr;
	}
	if (Colours)
	{
		delete[] Colours;
		Colours = nullptr;
	}
}

void ComponentMesh::ImportMesh(const aiMesh* mesh, uint MATTextureID)
{
	// --- Vertices ---
	this->VerticesSize = mesh->mNumVertices;
	this->Vertices = new float3[mesh->mNumVertices];

	for (uint i = 0; i < mesh->mNumVertices; ++i)
	{
		Vertices[i].x = mesh->mVertices[i].x;
		Vertices[i].y = mesh->mVertices[i].y;
		Vertices[i].z = mesh->mVertices[i].z;
	}

	glGenBuffers(1, (GLuint*)&this->VerticesID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->VerticesID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * this->VerticesSize, this->Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Normals ---
	if (mesh->HasNormals())
	{
		NormalsSize = mesh->mNumVertices;
		Normals = new float3[NormalsSize];

		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			Normals[i].x = mesh->mNormals[i].x;
			Normals[i].y = mesh->mNormals[i].y;
			Normals[i].z = mesh->mNormals[i].z;
		}

	}

	// --- Texture Coordinates ---

	if (mesh->HasTextureCoords(0))
	{
		TexCoords = new float[mesh->mNumVertices * 2];

		for (uint j = 0; j < mesh->mNumVertices; ++j)
		{
			TexCoords[j * 2] = mesh->mTextureCoords[0][j].x;
			TexCoords[(j * 2) + 1] = mesh->mTextureCoords[0][j].y;
		}
		/*LOG("Mesh texture coords at channel 0 loaded");*/
	}

	glGenBuffers(1, (GLuint*)&this->TextureCoordsID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->TextureCoordsID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->VerticesSize*2, this->TexCoords, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Colours ---

	if (mesh->HasVertexColors(0))
	{
		ColoursSize = mesh->mNumVertices;
		Colours = new unsigned char[ColoursSize * 4];

		for (uint i = 0; i < mesh->mNumVertices; ++i)
		{
			Colours[4*i] = mesh->mColors[0][i].r;
			Colours[(4*i)+1] = mesh->mColors[0][i].g;
			Colours[(4*i)+2] = mesh->mColors[0][i].b;
			Colours[(4*i)+3] = mesh->mColors[0][i].a;
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

	glGenBuffers(1, (GLuint*)&this->IndicesID); // create buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->IndicesID); // start using created buffer
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * this->IndicesSize, this->Indices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Texture ---

	this->TextureID = MATTextureID;

}



