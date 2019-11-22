#include "ImporterMesh.h"
#include "Application.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"
#include "ModuleFileSystem.h"
#include "ResourceMesh.h"

#include "Assimp/include/scene.h"

#include "OpenGL.h"
#include "Math.h"

#include "mmgr/mmgr.h"

ImporterMesh::ImporterMesh()
{
}

ImporterMesh::~ImporterMesh()
{
}

bool ImporterMesh::Import(const ImportData & IData) const
{

	ImportMeshData data = (ImportMeshData&) IData;


	// --- Vertices ---
	data.new_mesh->VerticesSize = data.mesh->mNumVertices;
	data.new_mesh->Vertices = new float3[data.mesh->mNumVertices];


	for (uint i = 0; i < data.mesh->mNumVertices; ++i)
	{
		data.new_mesh->Vertices[i].x = data.mesh->mVertices[i].x;
		data.new_mesh->Vertices[i].y = data.mesh->mVertices[i].y;
		data.new_mesh->Vertices[i].z = data.mesh->mVertices[i].z;
	}

	data.new_mesh->VerticesID = App->renderer3D->CreateBufferFromData(GL_ARRAY_BUFFER, sizeof(float3) * data.new_mesh->VerticesSize, data.new_mesh->Vertices);

	// --- Normals ---
	if (data.mesh->HasNormals())
	{
		data.new_mesh->NormalsSize = data.mesh->mNumVertices;
		data.new_mesh->Normals = new float3[data.new_mesh->NormalsSize];

		for (uint i = 0; i < data.mesh->mNumVertices; ++i)
		{
			data.new_mesh->Normals[i].x = data.mesh->mNormals[i].x;
			data.new_mesh->Normals[i].y = data.mesh->mNormals[i].y;
			data.new_mesh->Normals[i].z = data.mesh->mNormals[i].z;
		}

	}

	// --- Texture Coordinates ---
	data.new_mesh->TexCoordsSize = data.mesh->mNumVertices * 2;

	if (data.mesh->HasTextureCoords(0))
	{
		data.new_mesh->TexCoords = new float[data.new_mesh->TexCoordsSize];

		for (uint j = 0; j < data.mesh->mNumVertices; ++j)
		{
			data.new_mesh->TexCoords[j * 2] = data.mesh->mTextureCoords[0][j].x;
			data.new_mesh->TexCoords[(j * 2) + 1] = data.mesh->mTextureCoords[0][j].y;
		}
	}

	data.new_mesh->TextureCoordsID = App->renderer3D->CreateBufferFromData(GL_ARRAY_BUFFER, sizeof(float) * data.new_mesh->VerticesSize * 2, data.new_mesh->TexCoords);

	// --- Indices ---
	data.new_mesh->IndicesSize = data.mesh->mNumFaces * 3;
	data.new_mesh->Indices = new uint[data.new_mesh->IndicesSize];

	for (unsigned j = 0; j < data.mesh->mNumFaces; ++j)
	{
		const aiFace& face = data.mesh->mFaces[j];

		// Only triangles
		if (face.mNumIndices > 3)
		{
			CONSOLE_LOG("|[error]: Importer Mesh found a quad in %s, ignoring it. ", data.mesh->mName);
			continue;
		}

		data.new_mesh->Indices[j * 3] = face.mIndices[0];
		data.new_mesh->Indices[j * 3 + 1] = face.mIndices[1];
		data.new_mesh->Indices[j * 3 + 2] = face.mIndices[2];
	}

	data.new_mesh->IndicesID = App->renderer3D->CreateBufferFromData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * data.new_mesh->IndicesSize, data.new_mesh->Indices);

	data.new_mesh->CreateAABB();

	return true;
}

void ImporterMesh::Save(ResourceMesh * mesh, const char* path) const
{
	// amount of indices / vertices / normals / texture_coords / AABB
	uint ranges[4] = { mesh->IndicesSize, mesh->VerticesSize, mesh->NormalsSize, mesh->TexCoordsSize };

	uint size = sizeof(ranges) + sizeof(uint) * mesh->IndicesSize + sizeof(float)* 3 * mesh->VerticesSize + sizeof(float)*3*mesh->NormalsSize + sizeof(float)* mesh->TexCoordsSize;

	char* data = new char[size]; // Allocate
	char* cursor = data;

	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	// --- Store Indices ---
	cursor += bytes; 
	bytes = sizeof(uint) * mesh->IndicesSize;
	memcpy(cursor, mesh->Indices, bytes);

	// --- Store Vertices ---
	cursor += bytes; 
	bytes = sizeof(float) * mesh->VerticesSize*3;
	memcpy(cursor, mesh->Vertices, bytes);

	// --- Store Normals ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->NormalsSize*3;
	memcpy(cursor, mesh->Normals, bytes);

	// --- Store TexCoords ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->TexCoordsSize;

	if(mesh->TexCoords)
	memcpy(cursor, mesh->TexCoords, bytes);
	
	App->fs->Save(path, data, size);

	// --- Delete buffer data ---
	if (data)
	{
		delete[] data;
		data = nullptr;
		cursor = nullptr;
	}
}

void ImporterMesh::Load(const char * filename, ResourceMesh & mesh) const
{

	mesh.SetOriginalFilename(filename);

	// --- Load mesh data ---
	char* buffer;
	App->fs->Load(filename,&buffer);
	char* cursor = buffer;

	// amount of indices / vertices / normals / texture_coords
	uint ranges[4];
	uint bytes = sizeof(ranges);
	memcpy(ranges, cursor, bytes);

	mesh.IndicesSize = ranges[0];
	mesh.VerticesSize = ranges[1];
	mesh.NormalsSize = ranges[2];
	mesh.TexCoordsSize = ranges[3];

	// --- Load indices ---
	cursor += bytes;
	bytes = sizeof(uint) * mesh.IndicesSize;
	mesh.Indices = new uint[mesh.IndicesSize];
	memcpy(mesh.Indices, cursor, bytes);

	mesh.IndicesID = App->renderer3D->CreateBufferFromData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh.IndicesSize, mesh.Indices);

	// --- Load Vertices ---
	cursor += bytes;
	bytes = sizeof(float)*3 * mesh.VerticesSize;
	mesh.Vertices = new float3[mesh.VerticesSize];
	memcpy(mesh.Vertices, cursor, bytes);

	mesh.VerticesID = App->renderer3D->CreateBufferFromData(GL_ARRAY_BUFFER, sizeof(float3) * mesh.VerticesSize, mesh.Vertices);

	// --- Load Normals ---
	cursor += bytes;
	bytes = sizeof(float)*3 * mesh.NormalsSize;
	mesh.Normals = new float3[mesh.NormalsSize];
	memcpy(mesh.Normals, cursor, bytes);

	// --- Load Texture Coords ---
	cursor += bytes;
	bytes = sizeof(float) * mesh.TexCoordsSize;
	mesh.TexCoords = new float[mesh.TexCoordsSize];
	memcpy(mesh.TexCoords, cursor, bytes);

	mesh.TextureCoordsID = App->renderer3D->CreateBufferFromData(GL_ARRAY_BUFFER, sizeof(float) * mesh.TexCoordsSize, mesh.TexCoords);

	// --- Delete buffer data ---
	if (buffer)
	{
		delete[] buffer;
		buffer = nullptr;
		cursor = nullptr;
	}

	mesh.CreateAABB();
}

