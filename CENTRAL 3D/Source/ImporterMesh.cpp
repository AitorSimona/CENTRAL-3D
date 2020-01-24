#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceMesh.h"

#include "Assimp/include/scene.h"

#include "Math.h"

#include "mmgr/mmgr.h"

ImporterMesh::ImporterMesh() : Importer(Importer::ImporterType::Mesh)
{
}

ImporterMesh::~ImporterMesh()
{
}

Resource* ImporterMesh::Import(ImportData& IData) const
{
	ImportMeshData data = (ImportMeshData&)IData;

	// COULD USE MEMCPY AGAIN, JUST DECLARE AND ALLOCATE EVERYTHING FIRST

	data.new_mesh->vertices = new Vertex[data.mesh->mNumVertices];
	data.new_mesh->VerticesSize = data.mesh->mNumVertices;
	
	data.new_mesh->IndicesSize = data.mesh->mNumFaces * 3;
	data.new_mesh->Indices = new uint[data.new_mesh->IndicesSize];

	for (uint i = 0; i < data.mesh->mNumVertices; ++i)
	{
		// --- Vertices ---
		data.new_mesh->vertices[i].position[0] = data.mesh->mVertices[i].x;
		data.new_mesh->vertices[i].position[1] = data.mesh->mVertices[i].y;
		data.new_mesh->vertices[i].position[2] = data.mesh->mVertices[i].z;

		// --- Normals ---
		if (data.mesh->HasNormals())
		{
			data.new_mesh->vertices[i].normal[0] = data.mesh->mNormals[i].x;
			data.new_mesh->vertices[i].normal[1] = data.mesh->mNormals[i].y;
			data.new_mesh->vertices[i].normal[2] = data.mesh->mNormals[i].z;
		}

		// --- Colors ---
		if (data.mesh->HasVertexColors(0))
		{
			data.new_mesh->vertices[i].color[0] = data.mesh->mColors[0][i].r;
			data.new_mesh->vertices[i].color[1] = data.mesh->mColors[0][i].g;
			data.new_mesh->vertices[i].color[2] = data.mesh->mColors[0][i].b;
			data.new_mesh->vertices[i].color[3] = data.mesh->mColors[0][i].a;
		}

		// --- Texture Coordinates ---
		if (data.mesh->HasTextureCoords(0))
		{
			data.new_mesh->vertices[i].texCoord[0] = data.mesh->mTextureCoords[0][i].x;
			data.new_mesh->vertices[i].texCoord[1] = data.mesh->mTextureCoords[0][i].y;
		}
	}

	// --- Indices ---
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
		data.new_mesh->Indices[(j * 3) + 1] = face.mIndices[1];
		data.new_mesh->Indices[(j * 3) + 2] = face.mIndices[2];
	}


	/*data.new_mesh->LoadInMemory();
	data.new_mesh->CreateAABB();*/

	return nullptr;
}

void ImporterMesh::Save(ResourceMesh * mesh, const char* path) const
{
	// amount of indices / vertices / normals / texture_coords / AABB
	uint ranges[2] = { mesh->IndicesSize, mesh->VerticesSize};

	uint size = sizeof(ranges) + sizeof(uint) * mesh->IndicesSize + sizeof(float) * 3 * mesh->VerticesSize + sizeof(float) * 3 * mesh->VerticesSize + sizeof(unsigned char) * 4 * mesh->VerticesSize + sizeof(float) * 2 * mesh->VerticesSize;

	char* data = new char[size]; // Allocate
	float* Vertices = new float[mesh->VerticesSize*3];
	float* Normals = new float[mesh->VerticesSize*3];
	unsigned char* Colors = new unsigned char[mesh->VerticesSize*4];
	float* TexCoords = new float[mesh->VerticesSize*2];
	char* cursor = data;

	// --- Fill temporal arrays ---

	for (uint i = 0; i < mesh->VerticesSize; ++i)
	{
		// --- Vertices ---
		Vertices[i * 3] =	mesh->vertices[i].position[0];
		Vertices[(i * 3) + 1] = mesh->vertices[i].position[1];
		Vertices[(i * 3) + 2] = mesh->vertices[i].position[2];

		// --- Normals ---
		Normals[i * 3] = mesh->vertices[i].normal[0];
		Normals[(i * 3) + 1] = mesh->vertices[i].normal[1];
		Normals[(i * 3) + 2] = mesh->vertices[i].normal[2];

		// --- Colors ---
		Colors[i * 4] = mesh->vertices[i].color[0];
		Colors[(i * 4) + 1] = mesh->vertices[i].color[1];
		Colors[(i * 4) + 2] = mesh->vertices[i].color[2];
		Colors[(i * 4) + 3] = mesh->vertices[i].color[3];

		// --- Texture Coordinates ---
		TexCoords[i * 2] = mesh->vertices[i].texCoord[0];
		TexCoords[(i * 2) + 1] = mesh->vertices[i].texCoord[1];
	}


	// --- Store everything ---
	uint bytes = sizeof(ranges); // First store ranges
	memcpy(cursor, ranges, bytes);

	// --- Store Indices ---
	cursor += bytes; 
	bytes = sizeof(uint) * mesh->IndicesSize;
	memcpy(cursor, mesh->Indices, bytes);

	// --- Store Vertices ---
	cursor += bytes; 
	bytes = sizeof(float) * mesh->VerticesSize * 3;
	memcpy(cursor,Vertices, bytes);

	// --- Store Normals ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->VerticesSize * 3;
	memcpy(cursor, Normals, bytes);

	// --- Store Colors ---
	cursor += bytes;
	bytes = sizeof(unsigned char) * mesh->VerticesSize * 4;
	memcpy(cursor, Colors, bytes);

	// --- Store TexCoords ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->VerticesSize * 2;
	memcpy(cursor, TexCoords, bytes);
	
	App->fs->Save(path, data, size);

	// --- Delete buffer data ---
	if (data)
	{
		delete[] data;
		data = nullptr;
		cursor = nullptr;
	}

	delete[] Vertices;
	delete[] Normals;
	delete[] Colors;
	delete[] TexCoords;
}

Resource* ImporterMesh::Load(const char * path) const
{
	//mesh.SetOriginalFilename(path);

	//// --- Load mesh data ---
	//char* buffer;
	//App->fs->Load(path,&buffer);
	//char* cursor = buffer;

	//// amount of indices / vertices / normals / texture_coords
	//uint ranges[2];
	//uint bytes = sizeof(ranges);
	//memcpy(ranges, cursor, bytes);

	//mesh.IndicesSize = ranges[0];
	//mesh.VerticesSize = ranges[1];

	//mesh.vertices = new Vertex[mesh.VerticesSize];
	//float* Vertices = new float[mesh.VerticesSize * 3];
	//float* Normals = new float[mesh.VerticesSize * 3];
	//unsigned char* Colors = new unsigned char[mesh.VerticesSize * 4];
	//float* TexCoords = new float[mesh.VerticesSize * 2];

	//// --- Load indices ---
	//cursor += bytes;
	//bytes = sizeof(uint) * mesh.IndicesSize;
	//mesh.Indices = new uint[mesh.IndicesSize];
	//memcpy(mesh.Indices, cursor, bytes);

	//// --- Load Vertices ---
	//cursor += bytes;
	//bytes = sizeof(float) * 3 * mesh.VerticesSize;
	//memcpy(Vertices, cursor, bytes);

	//// --- Load Normals ---
	//cursor += bytes;
	//bytes = sizeof(float)* 3 * mesh.VerticesSize;
	//memcpy(Normals, cursor, bytes);

	//// --- Load Colors ---
	//cursor += bytes;
	//bytes = sizeof(unsigned char) * 4 * mesh.VerticesSize;
	//memcpy(Colors, cursor, bytes);

	//// --- Load Texture Coords ---
	//cursor += bytes;
	//bytes = sizeof(float) * 2 * mesh.VerticesSize;
	//memcpy(TexCoords, cursor, bytes);

	//// --- Fill Vertex array ---

	//for (uint i = 0; i < mesh.VerticesSize; ++i)
	//{
	//	// --- Vertices ---
	//	mesh.vertices[i].position[0] = Vertices[i * 3];
	//	mesh.vertices[i].position[1] = Vertices[(i * 3) + 1];
	//	mesh.vertices[i].position[2] = Vertices[(i * 3) + 2];

	//	// --- Normals ---
	//	mesh.vertices[i].normal[0] = Normals[i * 3];
	//	mesh.vertices[i].normal[1] = Normals[(i * 3) + 1];
	//	mesh.vertices[i].normal[2] = Normals[(i * 3) + 2];

	//	// --- Colors ---
	//	mesh.vertices[i].color[0] = Colors[i * 4];
	//	mesh.vertices[i].color[1] = Colors[(i * 4) + 1];
	//	mesh.vertices[i].color[2] = Colors[(i * 4) + 2];
	//	mesh.vertices[i].color[3] = Colors[(i * 4) + 3];

	//	// --- Texture Coordinates ---
	//	mesh.vertices[i].texCoord[0] = TexCoords[i * 2];
	//	mesh.vertices[i].texCoord[1] = TexCoords[(i * 2) + 1];
	//}


	//// --- Delete buffer data ---
	//if (buffer)
	//{
	//	delete[] buffer;
	//	buffer = nullptr;
	//	cursor = nullptr;
	//}

	//delete[] Vertices;
	//delete[] Normals;
	//delete[] Colors;
	//delete[] TexCoords;

	//mesh.LoadInMemory();
	//mesh.CreateAABB();

	return nullptr;
}

