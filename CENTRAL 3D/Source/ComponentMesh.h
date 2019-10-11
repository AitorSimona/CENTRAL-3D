#ifndef __COMPONENT_MESH_H__
#define __COMPONENT_MESH_H__

#include "Component.h"
#include "Math.h"

struct aiMesh;


class ComponentMesh : public Component
{
public:

	ComponentMesh();
	virtual ~ComponentMesh();

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

	uint ImageName = 0;
};

#endif

