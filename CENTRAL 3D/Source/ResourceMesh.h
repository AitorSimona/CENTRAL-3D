#ifndef __RESOURCE_MESH__
#define __RESOURCE_MESH__

#include "Resource.h"
#include "Globals.h"
#include "MathGeoLib/include/Geometry/AABB.h"

class ResourceMesh : public Resource
{
public:
	// Struct that contains all vertex related data
	struct Vertex
	{
		float position[3];
		float normal[3];
		float color[4];
		float texCoord[2];
	};

public:

	ResourceMesh();
	~ResourceMesh();

	void CreateAABB();

	void LoadInMemory();
	void FreeMemory();


private:
	void CreateVBO(); 
	void CreateEBO(); 
	void CreateVAO(); 

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

	// --- New shader approach ---
	Vertex* vertices = nullptr;
	uint verticesSize = 0;

	uint * indices = nullptr;
	uint indicesSize = 0;

	uint VBO = 0;	// Vertex Buffer Object
	uint EBO = 0;	// Element/Index Buffer Object
	uint VAO = 0;	// Vertex Array Object
};

#endif // __RESOURCE_MESH__

