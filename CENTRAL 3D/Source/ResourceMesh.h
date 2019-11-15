#ifndef __RESOURCE_MESH__
#define __RESOURCE_MESH__

#include "Resource.h"
#include "Globals.h"
#include "MathGeoLib/include/Geometry/AABB.h"

class ResourceMesh : public Resource
{
public:

	ResourceMesh();
	~ResourceMesh();

	void CreateAABB();

	void LoadOnMemory();
	void FreeMemory();

public:

	float3* Vertices = nullptr;
	uint VerticesID = 0; // unique vertex in VRAM
	uint VerticesSize = 0;

	uint* Indices = nullptr;
	uint IndicesID = 0; // index in VRAM
	uint IndicesSize = 0;

	float3* Normals = nullptr;
	uint NormalsSize = 0;

	float* TexCoords = nullptr;
	uint  TextureCoordsID = 0;
	uint TexCoordsSize = 0;

	AABB aabb;
};

#endif // __RESOURCE_MESH__

