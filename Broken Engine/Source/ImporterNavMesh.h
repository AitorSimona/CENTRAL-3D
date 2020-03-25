#ifndef __IMPORTERNAVMESH_H__
#define __IMPORTERNAVMESH_H__
#pragma once
#include "Importer.h"
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE

//Custom struct that mirror dtNavMeshParams to avoid including detour
struct BROKEN_API be_dtNavMeshParams {
	float orig[3];					///< The world space origin of the navigation mesh's tile space. [(x, y, z)]
	float tileWidth;				///< The width of each tile. (Along the x-axis.)
	float tileHeight;				///< The height of each tile. (Along the z-axis.)
	int maxTiles;					///< The maximum number of tiles the navigation mesh can contain.
	int maxPolys;					///< The maximum number of polygons each tile can contain.
};

struct BROKEN_API NavMeshSetHeader {
	int magic;
	int version;
	int numTiles;
	be_dtNavMeshParams params;
};

struct BROKEN_API NavMeshTileHeader {
	unsigned int tileRef;
	int dataSize;
};

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

class BROKEN_API ImporterNavMesh : public Importer {
public:
	ImporterNavMesh();
	virtual ~ImporterNavMesh();

	Resource* Import(ImportData& IData) const override;

	void Save(class ResourceNavMesh* navmesh) const;
	Resource* Load(const char* path) const override;

	static inline Importer::ImporterType GetType() { return Importer::ImporterType::NavMesh; };
};

BE_END_NAMESPACE

#endif
