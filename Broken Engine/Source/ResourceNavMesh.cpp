#include "ResourceNavMesh.h"
#include "DetourNavMesh.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleFileSystem.h"
#include "ModuleResourceManager.h"
#include "ImporterNavMesh.h"

using namespace Broken;



ResourceNavMesh::ResourceNavMesh(uint UID, const char* source_file) : Resource(Resource::ResourceType::NAVMESH, UID, source_file) {
	extension = ".navmesh";
	resource_file = source_file;
	original_file = resource_file;

	previewTexID = App->gui->navmeshTexID;

}

ResourceNavMesh::~ResourceNavMesh() {
}

bool ResourceNavMesh::LoadInMemory() {
	bool ret = true;

	if (App->fs->Exists(resource_file.c_str())) {
		// --- Load navmesh data ---
		char* buffer = nullptr;
		App->fs->Load(resource_file.c_str(), &buffer);
		char* index = buffer;

		// Read header.
		NavMeshSetHeader header;
		errno_t readRes = memcpy_s(&header, sizeof(NavMeshSetHeader), index, sizeof(NavMeshSetHeader));
		index += sizeof(NavMeshSetHeader);

		if (readRes != 0)
			return false;

		if (header.magic != NAVMESHSET_MAGIC)
			return false;

		if (header.version != NAVMESHSET_VERSION)
			return false;

		navMesh = dtAllocNavMesh();
		if (!navMesh)
			return false;

		dtNavMeshParams params;
		memcpy(&params, &header.params, sizeof(dtNavMeshParams));

		dtStatus status = navMesh->init(&params);
		if (dtStatusFailed(status))
			return false;

		// Read tiles.
		for (int i = 0; i < header.numTiles; ++i) {
			NavMeshTileHeader tileHeader;
			readRes = memcpy_s(&tileHeader, sizeof(NavMeshTileHeader), index, sizeof(NavMeshTileHeader));
			index += sizeof(NavMeshTileHeader);
			if (readRes != 0)
				return false;

			if (!tileHeader.tileRef || !tileHeader.dataSize)
				break;

			unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
			if (!data) break;
			memset(data, 0, tileHeader.dataSize);
			readRes = memcpy_s(data, tileHeader.dataSize, index, tileHeader.dataSize);
			index += tileHeader.dataSize;
			if (readRes != 0) {
				dtFree(data);
				return false;
			}

			navMesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
		}

		if (buffer != nullptr)
			delete[] buffer;

	}

	return ret;
}

void ResourceNavMesh::FreeMemory() {
	dtFreeNavMesh(navMesh);
	navMesh = nullptr;
}

void ResourceNavMesh::OnOverwrite() {
	NotifyUsers(ResourceNotificationType::Overwrite);
}

void ResourceNavMesh::OnDelete() {
	NotifyUsers(ResourceNotificationType::Deletion);

	FreeMemory();
	App->fs->Remove(resource_file.c_str());

	App->resources->RemoveResourceFromFolder(this);
	App->resources->ONResourceDestroyed(this);
}
