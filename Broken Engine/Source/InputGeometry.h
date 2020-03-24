#ifndef __INPUTGEOMETRY_H__
#define __INPUTGEOMETRY_H__
#pragma once

#include "GameObject.h"
#include "ResourceMesh.h"

static const int MAX_CONVEXVOL_PTS = 12;
struct ConvexVolume {
	float verts[MAX_CONVEXVOL_PTS * 3];
	float hmin, hmax;
	int nverts;
	int area;
};

class InputGeom {
public:
	/**
	*Create recast compatible inputgeom from the specified entities.The entities have to be added to the
	* scene before this call, as we need to calculate the world coordinates of the entity.
	* Verticesand faces of the specified source entities are stored in this inputGeom, individual entity
	* groupingand origin points are lost.
	**/
	InputGeom(const std::vector<Broken::GameObject*>& srcMeshes);
	InputGeom(Broken::GameObject* srcMesh);
	~InputGeom();

	/**
	 * Retrieves the vertices stored within this inputGeom. The verts are an array of floats in which each
	 * subsequent three floats are in order the x, y and z coordinates of a vert. The size of this array is
	 * always a multiple of three and is exactly 3*getVertCount().
	 **/
	const float* getVerts();
	/**
	* The number of vertices stored in this inputGeom.
	**/
	uint getVertCount();

	/**
	* Retrieves the tris stored in this inputGeom.
	* A tri is defined by a sequence of three indexes which refer to an index position in the getVerts() array.
	* Similar to getVerts, the size of this array is a multitude of 3 and is exactly 3*getTriCount().
	**/
	const int* getTris();

	/**
	* The number of triangles stored in this inputGeom.
	**/
	int getTriCount();

	/**
	* Retrieve the normals calculated for this inputGeom. Note that the normals are not exact and are not meant for rendering,
	* but they are good enough for navmesh calculation. Each normal corresponds to one vertex from getVerts() with the same index.
	* The size of the normals array is 3*getVertCount().
	**/
	const float* getNormals();

	/**
	* The axis aligned bounding box minimum of this input Geom.
	**/
	const float* getMeshBoundsMin();

	/**
	* The axis aligned bounding box maximum of this input Geom.
	**/
	const float* getMeshBoundsMax();

	/**
	* Wether this inputgeom has geometry
	**/
	bool hasMesh();

	/**
	* The amount of convex volumes
	**/
	int getConvexVolumeCount() const { return m_volumeCount; }

	/**
	* Pointer to the convex volumes array
	**/
	const ConvexVolume* getConvexVolumes() const { return m_volumes; }

	int getOffMeshConnectionCount() const { return m_offMeshConCount; }
	float* getOffMeshConnectionVerts() const { return (float*)m_offMeshConVerts; }
	float* getOffMeshConnectionRads() const { return (float*)m_offMeshConRads; }
	unsigned char* getOffMeshConnectionDirs() const { return (unsigned char*)m_offMeshConDirs; }
	unsigned char* getOffMeshConnectionAreas() const { return (unsigned char*)m_offMeshConAreas; }
	unsigned short* getOffMeshConnectionFlags() const { return (unsigned short*)m_offMeshConFlags; }
	unsigned int* getOffMeshConnectionId() const { return (unsigned int*)m_offMeshConId; }

private:
	void ApplyTransform(const Broken::Vertex& vertex, const float4x4& transform, float ret[3]);

private:
	bool m_mesh;

	float* verts;
	int nverts;

	int* tris;
	int ntris;

	float* normals;

	float* bmin;
	float* bmax;

	// Not implemented yet
	/// @name Off-Mesh connections.
	///@{
	static const int MAX_OFFMESH_CONNECTIONS = 256;
	float m_offMeshConVerts[MAX_OFFMESH_CONNECTIONS * 3 * 2];
	float m_offMeshConRads[MAX_OFFMESH_CONNECTIONS];
	unsigned char m_offMeshConDirs[MAX_OFFMESH_CONNECTIONS];
	unsigned char m_offMeshConAreas[MAX_OFFMESH_CONNECTIONS];
	unsigned short m_offMeshConFlags[MAX_OFFMESH_CONNECTIONS];
	unsigned int m_offMeshConId[MAX_OFFMESH_CONNECTIONS];
	int m_offMeshConCount = 0;
	///@}

	/// @name Convex Volumes (temporary) added to this geometry.
	///@{
	static const int MAX_VOLUMES = 256;
	ConvexVolume m_volumes[MAX_VOLUMES];
	int m_volumeCount;
	///@}


};

#endif

