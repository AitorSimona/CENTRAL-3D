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
	vec _bmin, _bmax;
	bool minmaxset = false;
	
	std::vector<float> v_verts;
	std::vector<int> v_tris;

	float4x4 transform;

	for (std::vector<Broken::GameObject*>::const_iterator it = srcMeshes.cbegin(); it != srcMeshes.cend(); ++it) {
		r_mesh = (*it)->GetComponent<Broken::ComponentMesh>()->resource_mesh;
		transform = (*it)->GetComponent<Broken::ComponentTransform>()->GetGlobalTransform();
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
		memcpy(verts, v_verts.data(), v_verts.size() * sizeof(float));
		
		calculateNormals();
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

void InputGeom::calculateNormals() {
	normals = new float[ntris*3];
	for (int i = 0; i < ntris * 3; i += 3) {
		const float* v0 = &verts[tris[i] * 3];
		const float* v1 = &verts[tris[i + 1] * 3];
		const float* v2 = &verts[tris[i + 2] * 3];
		float e0[3], e1[3];
		for (int j = 0; j < 3; ++j) {
			e0[j] = v1[j] - v0[j];
			e1[j] = v2[j] - v0[j];
		}
		float* n = &normals[i];
		n[0] = e0[1] * e1[2] - e0[2] * e1[1];
		n[1] = e0[2] * e1[0] - e0[0] * e1[2];
		n[2] = e0[0] * e1[1] - e0[1] * e1[0];
		float d = sqrtf(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
		if (d > 0) {
			d = 1.0f / d;
			n[0] *= d;
			n[1] *= d;
			n[2] *= d;
		}
	}

}
