#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"

#include "ResourceMesh.h"

#include "Assimp/include/scene.h"

#include "Math.h"

#include "mmgr/mmgr.h"


using namespace Broken;
ImporterMesh::ImporterMesh() : Importer(Importer::ImporterType::Mesh) {
}

ImporterMesh::~ImporterMesh() {
}

Resource* ImporterMesh::Import(ImportData& IData) const {
	ImportMeshData data = (ImportMeshData&)IData;

	ResourceMesh* resource_mesh = (ResourceMesh*)App->resources->CreateResource(Resource::ResourceType::MESH, IData.path);

	resource_mesh->vertices = new Vertex[data.mesh->mNumVertices];
	resource_mesh->VerticesSize = data.mesh->mNumVertices;

	resource_mesh->IndicesSize = data.mesh->mNumFaces * 3;
	resource_mesh->Indices = new uint[resource_mesh->IndicesSize];

	for (uint i = 0; i < data.mesh->mNumVertices; ++i)
	{
		// --- Vertices ---
		memcpy(resource_mesh->vertices[i].position, &data.mesh->mVertices[i], sizeof(float) * 3);

		// --- Normals ---
		if (data.mesh->HasNormals()) {
			memcpy(resource_mesh->vertices[i].normal, &data.mesh->mNormals[i], sizeof(float) * 3);
		}

		// --- Colors ---
		if (data.mesh->HasVertexColors(0))
		{
			resource_mesh->vertices[i].color[0] = data.mesh->mColors[0][i].r;
			resource_mesh->vertices[i].color[1] = data.mesh->mColors[0][i].g;
			resource_mesh->vertices[i].color[2] = data.mesh->mColors[0][i].b;
			resource_mesh->vertices[i].color[3] = data.mesh->mColors[0][i].a;
		}

		// --- Texture Coordinates ---
		if (data.mesh->HasTextureCoords(0))
		{
			resource_mesh->vertices[i].texCoord[0] = data.mesh->mTextureCoords[0][i].x;
			resource_mesh->vertices[i].texCoord[1] = data.mesh->mTextureCoords[0][i].y;
		}

		// --- Tangents & Bitangents ---
		if (data.mesh->HasTangentsAndBitangents())
		{
			resource_mesh->vertices[i].tangent[0] = data.mesh->mTangents[i].x;
			resource_mesh->vertices[i].tangent[1] = data.mesh->mTangents[i].y;
			resource_mesh->vertices[i].tangent[2] = data.mesh->mTangents[i].z;

			resource_mesh->vertices[i].biTangent[0] = data.mesh->mBitangents[i].x;
			resource_mesh->vertices[i].biTangent[1] = data.mesh->mBitangents[i].y;
			resource_mesh->vertices[i].biTangent[2] = data.mesh->mBitangents[i].z;
		}
	}

	// --- Indices ---
	for (unsigned j = 0; j < data.mesh->mNumFaces; ++j) {
		const aiFace& face = data.mesh->mFaces[j];

		// Only triangles
		if (face.mNumIndices > 3) {
			ENGINE_CONSOLE_LOG("|[error]: Importer Mesh found a quad in %s, ignoring it. ", data.mesh->mName);
			continue;
		}

		resource_mesh->Indices[j * 3] = face.mIndices[0];
		resource_mesh->Indices[(j * 3) + 1] = face.mIndices[1];
		resource_mesh->Indices[(j * 3) + 2] = face.mIndices[2];
	}

	// --- Save to library ---
	Save(resource_mesh);

	App->resources->AddResourceToFolder(resource_mesh);

	return resource_mesh;
}

void ImporterMesh::Save(ResourceMesh* mesh) const {
	uint sourcefilename_length = std::string(mesh->GetOriginalFile()).size();

	// amount of indices / vertices / normals / texture_coords / AABB
	uint ranges[3] = { sourcefilename_length, mesh->IndicesSize, mesh->VerticesSize };

	uint size =   sizeof(ranges) + sizeof(const char) * sourcefilename_length
				+ sizeof(uint) * mesh->IndicesSize								//Size of indices
				+ sizeof(float) * 3 * mesh->VerticesSize						//Size of vertex positions
				+ sizeof(float) * 3 * mesh->VerticesSize						//Size of vertex normals
				+ sizeof(unsigned char) * 4 * mesh->VerticesSize				//Size of vertex color
				+ sizeof(float) * 2 * mesh->VerticesSize						//Size of texture coordinates
				+ sizeof(float) * 3 * mesh->VerticesSize						//Size of tangents
				+ sizeof(float) * 3 * mesh->VerticesSize;						//Size of bitangents


	char* data = new char[size]; // Allocate

	float* Vertices = new float[mesh->VerticesSize * 3];
	float* Normals = new float[mesh->VerticesSize * 3];
	unsigned char* Colors = new unsigned char[mesh->VerticesSize * 4];
	float* TexCoords = new float[mesh->VerticesSize * 2];

	float* Tangents = new float[mesh->VerticesSize * 3];
	float* BiTangents = new float[mesh->VerticesSize * 3];
	
	char* cursor = data;

	// --- Fill temporal arrays ---
	for (uint i = 0; i < mesh->VerticesSize; ++i) {
		// --- Vertices ---
		Vertices[i * 3] = mesh->vertices[i].position[0];
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

		// --- Tangents ---
		Tangents[i * 3] = mesh->vertices[i].tangent[0];
		Tangents[(i * 3) + 1] = mesh->vertices[i].tangent[1];
		Tangents[(i * 3) + 2] = mesh->vertices[i].tangent[2];

		// --- BiTangents ---
		BiTangents[i * 3] = mesh->vertices[i].biTangent[0];
		BiTangents[(i * 3) + 1] = mesh->vertices[i].biTangent[1];
		BiTangents[(i * 3) + 2] = mesh->vertices[i].biTangent[2];
	}


	// --- Store everything ---

	// --- Store ranges ---
	uint bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);

	// --- Store original filename ---
	cursor += bytes;
	bytes = sizeof(const char) * sourcefilename_length;
	memcpy(cursor, mesh->GetOriginalFile(), bytes);

	// --- Store Indices ---
	cursor += bytes;
	bytes = sizeof(uint) * mesh->IndicesSize;
	memcpy(cursor, mesh->Indices, bytes);

	// --- Store Vertices ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->VerticesSize * 3;
	memcpy(cursor, Vertices, bytes);

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

	// --- Store Tangents ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->VerticesSize * 3;
	memcpy(cursor, Tangents, bytes);

	// --- Store BiTangents ---
	cursor += bytes;
	bytes = sizeof(float) * mesh->VerticesSize * 3;
	memcpy(cursor, BiTangents, bytes);

	App->fs->Save(mesh->GetResourceFile(), data, size);

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
	delete[] Tangents;
	delete[] BiTangents;
}

Resource* ImporterMesh::Load(const char* path) const
{
	Resource* mesh = nullptr;
	char* buffer = nullptr;

	if (App->fs->Exists(path))
	{
		App->fs->Load(path, &buffer);

		if (buffer) {
			// --- Read ranges first ---
			char* cursor = buffer;
			uint ranges[3];
			uint bytes = sizeof(ranges);
			memcpy(ranges, cursor, bytes);

			// --- Read the original file's name ---
			std::string source_file;
			source_file.resize(ranges[0]);
			cursor += bytes;
			bytes = sizeof(char) * ranges[0];
			memcpy((char*)source_file.c_str(), cursor, bytes);

			// --- Extract UID from path ---
			std::string uid = path;
			App->fs->SplitFilePath(path, nullptr, &uid);
			uid = uid.substr(0, uid.find_last_of("."));


			mesh = App->resources->meshes.find(std::stoi(uid)) != App->resources->meshes.end() ? App->resources->meshes.find(std::stoi(uid))->second : App->resources->CreateResourceGivenUID(Resource::ResourceType::MESH, source_file.c_str(), std::stoi(uid));

			delete[] buffer;
			buffer = nullptr;
			cursor = nullptr;
		}

	}

	return mesh;
}

