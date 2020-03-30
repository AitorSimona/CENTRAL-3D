#include "InputGeometry.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Recast.h"
#include "EngineLog.h"
#include "mmgr/nommgr.h"


InputGeom::InputGeom(const std::vector<Broken::GameObject*>& srcMeshes) :
	nverts(0),
	ntris(0),
	maxtris(0),
	bmin(nullptr),
	bmax(nullptr),
	m_offMeshConCount(0),
	m_volumeCount(0),
	normals(nullptr),
	m_mesh(false)
{
	Broken::ResourceMesh* r_mesh;
	vec _bmin, _bmax;
	bool minmaxset = false;

	float4x4 transform;
	meshes.reserve(srcMeshes.size());
	for (std::vector<Broken::GameObject*>::const_iterator it = srcMeshes.cbegin(); it != srcMeshes.cend(); ++it) {
		r_mesh = (*it)->GetComponent<Broken::ComponentMesh>()->resource_mesh;
		transform = (*it)->GetComponent<Broken::ComponentTransform>()->GetGlobalTransform();
		r_mesh->CreateOBB();
		OBB t_obb = r_mesh->obb;
		t_obb.Transform(transform);
		AABB aabb = t_obb.MinimalEnclosingAABB();

		meshes.push_back(RecastMesh());

		// We add the index count
		meshes.back().tris = new int[r_mesh->IndicesSize];
		meshes.back().ntris = r_mesh->IndicesSize / 3;
		memcpy(meshes.back().tris, r_mesh->Indices, r_mesh->IndicesSize * sizeof(int));
		ntris += r_mesh->IndicesSize;
		if (meshes.back().ntris > maxtris)
			maxtris = meshes.back().ntris;

		// we add the transformed vertices
		nverts += r_mesh->VerticesSize;
		meshes.back().nverts = r_mesh->VerticesSize;
		meshes.back().verts = new float[r_mesh->VerticesSize * 3];
		float* vert_index = meshes.back().verts;
		float vert[3];
		for (int i = 0; i < r_mesh->VerticesSize; ++i) {
			ApplyTransform(r_mesh->vertices[i], transform, vert_index);
			vert_index += 3;
		}

		if (!minmaxset) {
			minmaxset = true;
			_bmin = aabb.minPoint;
			_bmax = aabb.maxPoint;
		}
		else {
			_bmin = math::Min(_bmin, aabb.minPoint);
			_bmax = math::Max(_bmax, aabb.maxPoint);
		}
		meshes.back().area = (*it)->navigationArea;

		if (meshes.back().area == 0)
			meshes.back().area = RC_WALKABLE_AREA;
		else if (meshes.back().area == 1)
			meshes.back().area = RC_NULL_AREA;

		m_mesh = true;
	}

	if (m_mesh) {
		ntris /= 3;
		bmin = new float[3];
		memcpy(bmin, _bmin.ptr(), 3 * sizeof(float));
		bmax = new float[3];
		memcpy(bmax, _bmax.ptr(), 3 * sizeof(float));
	}
}


InputGeom::~InputGeom() {
	if (bmin != nullptr)
		delete[] bmin;
	if (bmax != nullptr)
		delete[] bmax;
	if (normals != nullptr)
		delete[] normals;

	meshes.clear();
}

const std::vector<RecastMesh>& InputGeom::getMeshes() const {
	return meshes;
}

uint InputGeom::getVertCount() {
	return nverts;
}

int InputGeom::getTriCount() {
	return ntris;
}

int InputGeom::getMaxTris() {
	return maxtris;
}

const float* InputGeom::getNormals() {
	return normals;
}

const float* InputGeom::getMeshBoundsMin() {
	return bmin;
}

const float* InputGeom::getMeshBoundsMax() {
	return bmax;
}

bool InputGeom::hasMesh() {
	return m_mesh;
}

void InputGeom::ApplyTransform(const Broken::Vertex& vertex, const float4x4& transform, float ret[3]) {
	math::float3 original;
	memcpy(original.ptr(), vertex.position, sizeof(float) * 3);
	math::float3 globalVert = (transform * math::float4(vertex.position[0],
		vertex.position[1],
		vertex.position[2],
		1)).xyz();
	size_t a = sizeof(math::float3);

	ret[0] = globalVert[0];
	ret[1] = globalVert[1];
	ret[2] = globalVert[2];
}

RecastMesh::~RecastMesh() {
	if (verts != nullptr)
		delete[] verts;
	if (tris != nullptr)
		delete[] tris;
}
