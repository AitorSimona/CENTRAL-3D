#include "InputGeometry.h"
#include "ResourceMesh.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "Recast.h"
#include "EngineLog.h"
#include "mmgr/mmgr.h"


InputGeom::InputGeom(const std::vector<Broken::GameObject*>& srcMeshes) :
	nverts(0),
	ntris(0),
	bmin(nullptr),
	bmax(nullptr),
	m_offMeshConCount(0),
	m_volumeCount(0),
	normals(nullptr),
	verts(nullptr),
	tris(nullptr),
	m_mesh(false)
{
	Broken::ResourceMesh* r_mesh;
	Broken::ComponentTransform* comp_trans;
	vec _bmin, _bmax;
	bool minmaxset = false;
	
	std::vector<float> v_verts;
	std::vector<int> v_tris;

	for (std::vector<Broken::GameObject*>::const_iterator it = srcMeshes.cbegin(); it != srcMeshes.cend(); ++it) {
		r_mesh = (*it)->GetComponent<Broken::ComponentMesh>()->resource_mesh;
		comp_trans = (*it)->GetComponent<Broken::ComponentTransform>();
		float4x4 transform = comp_trans->GetGlobalTransform();
		const AABB& aabb = r_mesh->aabb;

		// If it is non-walkable we don't need to add its geometry
		if ((*it)->navigationArea != 1) {
			// We add the index count
			for (int i = 0; i < r_mesh->IndicesSize; ++i)
				v_tris.push_back(r_mesh->Indices[i] + ntris);
			ntris += r_mesh->IndicesSize;

			// we add the transformed vertices
			nverts += r_mesh->VerticesSize;
			float vert[3];
			for (int i = 0; i < r_mesh->VerticesSize; ++i) {
				ApplyTransform(r_mesh->vertices[i], transform, vert);
				v_verts.push_back(vert[0]);
				v_verts.push_back(vert[1]);
				v_verts.push_back(vert[2]);
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
			m_mesh = true;
		}

		if ((*it)->navigationArea != 0) {
			//Create convex hull for later flagging polys
			r_mesh->CreateOBB();
			OBB t_obb = r_mesh->obb;
			t_obb.Transform(transform);

			Polyhedron convexhull = t_obb.ToPolyhedron();

			m_volumes[m_volumeCount].area = (*it)->navigationArea;
			if (m_volumes[m_volumeCount].area == 1)
				m_volumes[m_volumeCount].area = RC_NULL_AREA;
			m_volumes[m_volumeCount].hmax = aabb.MaxY();
			m_volumes[m_volumeCount].hmin = aabb.MinY();
			m_volumes[m_volumeCount].nverts = convexhull.NumVertices();
			if (m_volumes[m_volumeCount].nverts < MAX_CONVEXVOL_PTS * 3) {
				memcpy(m_volumes[m_volumeCount].verts, convexhull.VertexArrayPtr()->ptr(), m_volumes[m_volumeCount].nverts * sizeof(float));
				m_volumeCount++;
			}
			else
				EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Convex hull has too many vertices");
		}
	}

	if (m_mesh) {
		ntris /= 3;
		bmin = new float[3];
		memcpy(bmin, _bmin.ptr(), 3 * sizeof(float));
		bmax = new float[3];
		memcpy(bmax, _bmax.ptr(), 3 * sizeof(float));

		tris = new int[v_tris.size()];
		memcpy(tris, v_tris.data(), v_tris.size() * sizeof(int));
		verts = new float[v_verts.size()];
		memcpy(verts, v_verts.data(), v_tris.size() * sizeof(float));
	}
}

InputGeom::InputGeom(Broken::GameObject* srcMesh) :
	nverts(0),
	ntris(0),
	bmin(nullptr),
	bmax(nullptr),
	m_offMeshConCount(0),
	m_volumeCount(0),
	normals(nullptr),
	verts(nullptr),
	tris(nullptr),
	m_mesh(false) 
{
	m_volumeCount = 0;
	Broken::ResourceMesh* r_mesh = srcMesh->GetComponent<Broken::ComponentMesh>()->resource_mesh;
	Broken::ComponentTransform* comp_trans = srcMesh->GetComponent<Broken::ComponentTransform>();
	float4x4 transform = comp_trans->GetGlobalTransform();
	const AABB& aabb = r_mesh->aabb;

	// If it is non-walkable we don't need to add its geometry
	if (srcMesh->navigationArea != 1) {
		// We add the index count
		ntris = r_mesh->IndicesSize;
		tris = new int[ntris];
		memcpy(tris, r_mesh->Indices, ntris * sizeof(int));

		// we add the transformed vertices
		nverts = r_mesh->VerticesSize;
		verts = new float[nverts];
		float* vert_index = verts;
		for (int i = 0; i < r_mesh->VerticesSize; ++i) {
			ApplyTransform(r_mesh->vertices[i], transform, vert_index);
			vert_index += 3;
		}

		bmin = new float[3];
		memcpy(bmin, aabb.minPoint.ptr(), 3 * sizeof(float));
		bmax = new float[3];
		memcpy(bmax, aabb.maxPoint.ptr(), 3 * sizeof(float));
		m_mesh = true;
	}

	if (srcMesh->navigationArea != 0) {
		//Create convex hull for later flagging polys
		r_mesh->CreateOBB();
		OBB t_obb = r_mesh->obb;
		t_obb.Transform(transform);

		Polyhedron convexhull = t_obb.ToPolyhedron();

		m_volumes[m_volumeCount].area = srcMesh->navigationArea;
		if (m_volumes[m_volumeCount].area == 1)
			m_volumes[m_volumeCount].area = RC_NULL_AREA;
		m_volumes[m_volumeCount].hmax = aabb.MaxY();
		m_volumes[m_volumeCount].hmin = aabb.MinY();
		m_volumes[m_volumeCount].nverts = convexhull.NumVertices();
		if (m_volumes[m_volumeCount].nverts < MAX_CONVEXVOL_PTS * 3) {
			memcpy(m_volumes[m_volumeCount].verts, convexhull.VertexArrayPtr()->ptr(), m_volumes[m_volumeCount].nverts);
			m_volumeCount++;
		}
		else
			EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Convex hull has too many vertices");
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
}

const float* InputGeom::getVerts() {
	return verts;
}

uint InputGeom::getVertCount() {
	return nverts;
}

const int* InputGeom::getTris() {
	return tris;
}

int InputGeom::getTriCount() {
	return ntris;
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
	float3 v;
	v.x = vertex.position[0];
	v.y = vertex.position[1];
	v.z = vertex.position[2];

	v = transform.MulPos(v);
	ret[0] = v[0];
	ret[1] = v[1];
	ret[2] = v[2];

}
