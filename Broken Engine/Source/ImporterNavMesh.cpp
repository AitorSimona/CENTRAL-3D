#include "ImporterNavMesh.h"
#include "DetourNavMesh.h"
#include "ResourceNavMesh.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "Application.h"

using namespace Broken;

ImporterNavMesh::ImporterNavMesh() : Importer(Importer::ImporterType::NavMesh) {
}

ImporterNavMesh::~ImporterNavMesh() {
}

Resource* ImporterNavMesh::Import(ImportData& IData) const {
	ResourceNavMesh* navmesh = (ResourceNavMesh*)App->resources->CreateResource(Resource::ResourceType::NAVMESH, IData.path);

	return navmesh;
}

void ImporterNavMesh::Save(ResourceNavMesh* navmesh) const {
	uint datasize = 0;
	char* data = nullptr;
	char* d_index = nullptr;
	NavMeshSetHeader* header = new NavMeshSetHeader();
	datasize += sizeof(NavMeshSetHeader);
	header->magic = NAVMESHSET_MAGIC;
	header->version = NAVMESHSET_VERSION;
	header->numTiles = 0;

	const dtNavMesh* mesh = navmesh->navMesh;
	for (int i = 0; i <mesh->getMaxTiles(); ++i) {
		const dtMeshTile* tile = (const dtMeshTile*)mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;
		header->numTiles++;
		datasize += sizeof(NavMeshTileHeader);
		datasize += tile->dataSize;
	}

	memcpy(&header->params, mesh->getParams(), sizeof(dtNavMeshParams));

	data = new char[datasize];
	d_index = data;

	// We write the header file
	memcpy(d_index, header, sizeof(NavMeshSetHeader));
	d_index += sizeof(NavMeshSetHeader);

	// Store tiles.
	for (int i = 0; i < mesh->getMaxTiles(); ++i) {
		const dtMeshTile* tile = mesh->getTile(i);
		if (!tile || !tile->header || !tile->dataSize) continue;

		NavMeshTileHeader tileHeader;
		tileHeader.tileRef = mesh->getTileRef(tile);
		tileHeader.dataSize = tile->dataSize;
		memcpy(d_index, &tileHeader, sizeof(NavMeshTileHeader));
		d_index += sizeof(NavMeshTileHeader);

		memcpy(d_index, tile->data, tile->dataSize);
		d_index += tile->dataSize;
	}

	App->fs->Save(navmesh->GetResourceFile(), data, datasize);

	if (data) {
		delete[] data;
		data = nullptr;
		d_index = nullptr;
	}

	delete header;
}

Resource* ImporterNavMesh::Load(const char* path) const {
	Resource* navmesh = nullptr;

	if (App->fs->Exists(path)) {

		// --- Extract UID from path ---
		std::string uid = path;
		App->fs->SplitFilePath(path, nullptr, &uid);
		uid = uid.substr(0, uid.find_last_of("."));


		navmesh = App->resources->navmeshes.find(std::stoi(uid)) != App->resources->navmeshes.end() ? App->resources->navmeshes.find(std::stoi(uid))->second : App->resources->CreateResourceGivenUID(Resource::ResourceType::NAVMESH, path, std::stoi(uid));
	}

	return navmesh;
}
