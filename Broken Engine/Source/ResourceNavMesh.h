#ifndef __RESOURCENAVMESH_H__
#define __RESOURCENAVMESH_H__
#pragma once

#include "Resource.h"
#include "BrokenCore.h"

class dtNavMesh;

BE_BEGIN_NAMESPACE
class BROKEN_API ResourceNavMesh : public Resource {
	friend class ModuleDetour;
public:
	ResourceNavMesh(uint UID, const char* source_file);
	~ResourceNavMesh();

	bool LoadInMemory() override;
	void FreeMemory() override;

private:
	void OnOverwrite() override;
	void OnDelete() override;

public:
	dtNavMesh* navMesh;
};

BE_END_NAMESPACE

#endif