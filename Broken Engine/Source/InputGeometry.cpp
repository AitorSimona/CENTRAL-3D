#include "InputGeometry.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Recast.h"
#include "EngineLog.h"
#include "mmgr/mmgr.h"

InputGeom::InputGeom(const std::vector<Broken::GameObject*>& srcMeshes, bool createChunky) :
	nverts(0),
	ntris(0),
	maxtris(0),
	bmin(nullptr),
	bmax(nullptr),
	m_offMeshConCount(0),
	m_volumeCount(0),
	normals(nullptr),
	verts(nullptr),
	tris(nullptr),
	areas(nullptr),
	m_mesh(false),
	chunkyTriMesh(nullptr)
{
	Broken::ResourceMesh* r_mesh;
	vec _bmin, _bmax;
	bool minmaxset = false;

	float4x4 transform;
	// First pass to allocate our arrays
	for (std::vector<Broken::GameObject*>::const_iterator it = srcMeshes.cbegin(); it != srcMeshes.cend(); ++it) {
		r_mesh = (*it)->GetComponent<Broken::ComponentMesh>()->resource_mesh;
		ntris += r_mesh->IndicesSize;
		nverts += r_mesh->VerticesSize;
	}


	meshes.reserve(srcMeshes.size());
	verts = new float[nverts * 3];
	tris = new int[ntris];
	areas= new uchar[ntris / 3];

	unsigned int t_verts = 0;
	unsigned int t_indices = 0;
	unsigned int t_tris = 0;
	for (std::vector<Broken::GameObject*>::const_iterator it = srcMeshes.cbegin(); it != srcMeshes.cend(); ++it) {
		m_mesh = true;
		r_mesh = (*it)->GetComponent<Broken::ComponentMesh>()->resource_mesh;
		transform = (*it)->GetComponent<Broken::ComponentTransform>()->GetGlobalTransform();
		r_mesh->CreateOBB();
		OBB t_obb = r_mesh->obb;
		t_obb.Transform(transform);
		AABB aabb = t_obb.MinimalEnclosingAABB();

		meshes.push_back(RecastMesh());

		// We add the index count
		meshes.back().tris = &tris[t_indices];
		meshes.back().ntris = r_mesh->IndicesSize / 3;

		// We do need to add the amount of vertices to the vertex they are pointing
		for (int i = 0; i < r_mesh->IndicesSize; ++i)
			meshes.back().tris[i] = r_mesh->Indices[i] + t_verts;

		t_indices += r_mesh->IndicesSize;
		if (meshes.back().ntris > maxtris)
			maxtris = meshes.back().ntris;

		// we add the transformed vertices
		meshes.back().nverts = r_mesh->VerticesSize;
		float* vert_index = &verts[t_verts * 3];
		for (int i = 0; i < r_mesh->VerticesSize; ++i) {
			ApplyTransform(r_mesh->vertices[i], transform, vert_index);
			vert_index += 3;
		}
		t_verts += r_mesh->VerticesSize;

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

		for (int i = 0; i < r_mesh->IndicesSize / 3; ++i)
			areas[t_tris + i] = meshes.back().area;
		t_tris += r_mesh->IndicesSize / 3;
	}

	if (m_mesh) {
		ntris /= 3;
		bmin = new float[3];
		memcpy(bmin, _bmin.ptr(), 3 * sizeof(float));
		bmax = new float[3];
		memcpy(bmax, _bmax.ptr(), 3 * sizeof(float));
	}

	if (createChunky) {
		chunkyTriMesh = new rcChunkyTriMesh();
		if (!rcCreateChunkyTriMesh(verts, tris, areas, ntris, 256, chunkyTriMesh))
			EX_ENGINE_CONSOLE_LOG("[InputGeo]: Could not create ChunkyTriMesh for tiling");
	}
}


InputGeom::~InputGeom() {
	if (bmin != nullptr)
		delete[] bmin;
	if (bmax != nullptr)
		delete[] bmax;
	if (normals != nullptr)
		delete[] normals;
	if (verts != nullptr)
		delete[] verts;
	if (tris != nullptr)
		delete[] tris;
	if (areas != nullptr)
		delete[] areas;
	if (chunkyTriMesh != nullptr)
		delete chunkyTriMesh;

	meshes.clear();
}

const std::vector<RecastMesh>& InputGeom::getMeshes() const {
	return meshes;
}

const float* InputGeom::getVerts() const {
	return verts;
}

uint InputGeom::getVertCount() const{
	return nverts;
}

int InputGeom::getTriCount() const{
	return ntris;
}

int InputGeom::getMaxTris() const{
	return maxtris;
}

const float* InputGeom::getNormals() const{
	return normals;
}

const float* InputGeom::getMeshBoundsMin() const{
	return bmin;
}

const float* InputGeom::getMeshBoundsMax() const{
	return bmax;
}

const rcChunkyTriMesh* InputGeom::getChunkyMesh() const {
	return chunkyTriMesh;
}

bool InputGeom::hasMesh() const{
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
}
