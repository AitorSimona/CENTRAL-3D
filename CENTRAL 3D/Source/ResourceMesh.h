#ifndef __RESOURCE_MESH_H__
#define __RESOURCE_MESH_H__

#include "Globals.h"
#include "Resource.h"
#include "Math.h"

struct aiMesh;

class ResourceMesh : public Resource
{
public:

	ResourceMesh();
	virtual ~ResourceMesh();

	void ImportMesh(aiMesh* mesh);

public:

	float3* Vertices = nullptr;
	uint VerticesID = 0; // unique vertex in VRAM
	uint VerticesSize = 0;

	uint* Indices = nullptr; 
	uint IndicesID = 0; // index in VRAM
	uint IndicesSize = 0;

	float3* Normals = nullptr;
	uint NormalsSize = 0;

	float2* TexCoords = nullptr;
	uint  TexID = 0;
	uint TexCoordsSize = 0;

	unsigned char* Colours = nullptr;
	uint ColoursSize = 0;

};

#endif

