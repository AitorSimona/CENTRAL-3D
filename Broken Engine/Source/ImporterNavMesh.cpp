#include "ImporterNavMesh.h"
#include "DetourNavMesh.h"
#include "ResourceNavMesh.h"
#include "ModuleResourceManager.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "ResourceMeta.h"
#include "ImporterMeta.h"

using namespace Broken;

ImporterNavMesh::ImporterNavMesh() : Importer(Importer::ImporterType::NavMesh) {
}

ImporterNavMesh::~ImporterNavMesh() {
}

Resource* ImporterNavMesh::Import(ImportData& IData) const {

	// --- Meta was deleted, just trigger a load with a new uid ---
	Resource* navmesh = Load(IData.path);

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
	ResourceNavMesh* navmesh = nullptr;

	// --- Load NavMesh file ---
	if (App->fs->Exists(path)) {
		ImporterMeta* IMeta = App->resources->GetImporter<ImporterMeta>();
		ResourceMeta* meta = (ResourceMeta*)IMeta->Load(path);

		if (meta) {
			navmesh = App->resources->navmeshes.find(meta->GetUID()) != App->resources->navmeshes.end() ? App->resources->navmeshes.find(meta->GetUID())->second : (ResourceNavMesh*)App->resources->CreateResourceGivenUID(Resource::ResourceType::NAVMESH, meta->GetOriginalFile(), meta->GetUID());
		}
		else {
			navmesh = (ResourceNavMesh*)App->resources->CreateResource(Resource::ResourceType::NAVMESH, path);
		}

	}

	return navmesh;
}
