#include "ComponentMesh.h"
#include "OpenGL.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#include "mmgr/mmgr.h"


ComponentMesh::ComponentMesh() : Component(Component::ComponentType::mesh)
{
}

ComponentMesh::~ComponentMesh()
{
	glDeleteBuffers(1, (GLuint*)&VerticesID);
	glDeleteBuffers(1, (GLuint*)&IndicesID);

	RELEASE_ARRAY(Vertices);
	RELEASE_ARRAY(Indices);
	RELEASE_ARRAY(Normals);
	RELEASE_ARRAY(TexCoords);
}

void ComponentMesh::ImportMesh(aiMesh* mesh)
{
	// --- Vertices ---
	this->VerticesSize = mesh->mNumVertices;
	this->Vertices = new float3[mesh->mNumVertices*3];

	memcpy(Vertices, mesh->mVertices, sizeof(float3) * VerticesSize * 3);

	glGenBuffers(1, (GLuint*)&this->VerticesID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->VerticesID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * this->VerticesSize*3, this->Vertices, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Normals ---
	if (mesh->HasNormals())
	{
		NormalsSize = mesh->mNumVertices;
		Normals = new float3[NormalsSize];
		memcpy(Normals, mesh->mNormals, sizeof(float3)*NormalsSize);
	}

	// --- Texture Coordinates ---

	if (mesh->HasTextureCoords(0))
	{
		TexCoords = new float[mesh->mNumVertices * 2];

		for (uint j = 0; j < mesh->mNumVertices; ++j)
		{
			memcpy(&TexCoords[j*2], &mesh->mTextureCoords[0][j].x, sizeof(float));
			memcpy(&TexCoords[(j*2)+1], &mesh->mTextureCoords[0][j].y, sizeof(float));
		}
		LOG("Mesh tex coords at channel 0 loaded");
	}


	glGenBuffers(1, (GLuint*)&this->TextureCoordsID); // create buffer
	glBindBuffer(GL_ARRAY_BUFFER, this->TextureCoordsID); // start using created buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * this->VerticesSize*2, this->TexCoords, GL_STATIC_DRAW); // send vertices to VRAM
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Stop using buffer

	// --- Colours ---

	if (mesh->HasVertexColors(0))
	{
		ColoursSize = VerticesSize;
		Colours = new unsigned char[ColoursSize * 4];
		memcpy(Colours, mesh->mColors, sizeof(unsigned char)*ColoursSize * 4);
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

}



