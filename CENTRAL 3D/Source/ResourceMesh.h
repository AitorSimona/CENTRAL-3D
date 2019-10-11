#ifndef __RESOURCE_MESH_H__
#define __RESOURCE_MESH_H__

#include "Resource.h"

struct aiMesh;


class ResourceMesh : public Resource
{
public:

	ResourceMesh();
	virtual ~ResourceMesh();

	void ImportMesh(aiMesh* mesh);

public:

	/*float* vertices = nullptr;
	uint VerticesSize = 0;
	uint VerticesID = 0;

	uint* Indices = nullptr;
	uint IndicesSize = 0;
	uint IndicesID = 0;

	float* textureCoords = nullptr;
	uint textureCoordsID = 0;*/

	float* Vertices = nullptr;
	uint VerticesID = 0; // unique vertex in VRAM
	uint VerticesSize = 0;

	uint* Indices = nullptr;
	uint IndicesID = 0; // index in VRAM
	uint IndicesSize = 0;

	float* Normals = nullptr;
	uint NormalsSize = 0;

	float* TexCoords = nullptr;
	uint  TexID = 0;
	uint TexCoordsSize = 0;

	unsigned char* Colours = nullptr;
	uint ColoursSize = 0;

	uint ImageName = 0;
};

#endif

