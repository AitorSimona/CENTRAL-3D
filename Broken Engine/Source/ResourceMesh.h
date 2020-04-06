#ifndef __RESOURCE_MESH__
#define __RESOURCE_MESH__

#include "Resource.h"
#include "MathGeoLib/include/Geometry/AABB.h"

BE_BEGIN_NAMESPACE
struct BROKEN_API Vertex
{
	float position[3];
	float normal[3];
	unsigned char color[4];
	float texCoord[2];
	float tangent[3];
	float biTangent[3];
};

class BROKEN_API ResourceMesh : public Resource
{
public:

	ResourceMesh(uint UID, const char* source_file);
	~ResourceMesh();

	void CreateAABB();

	bool LoadInMemory() override;
	void FreeMemory() override;

private:

	void CreateVBO();
	void CreateEBO();
	void CreateVAO();

public:

	AABB aabb;

	Vertex* vertices = nullptr;
	uint VerticesSize = 0;
	uint* Indices = nullptr;
	uint IndicesSize = 0;

	// --- New shader approach ---

	uint VBO = 0;	// Vertex Buffer Object
	uint EBO = 0;	// Element/Index Buffer Object

	uint VAO = 0;	// Vertex Array Object

private:

	void OnOverwrite() override;
	void OnDelete() override;
	void Repath() override;
};

BE_END_NAMESPACE
#endif // __RESOURCE_MESH__

