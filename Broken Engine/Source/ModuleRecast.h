#ifndef __MODULERECAST_H__
#define __MODULERECAST_H__
#pragma once

#include "Module.h"
#include "Recast.h"

class InputGeom;
namespace Broken
{
	class GameObject;
	struct Event;
}
namespace math
{
	class OBB;
}

/// The maximum number of vertices per navigation polygon.
/// @ingroup detour
//static const int DT_VERTS_PER_POLYGON = 6;

class ModuleRecast : public Broken::Module {
public:
	ModuleRecast(bool start_enabled = true);
	~ModuleRecast();

	bool Init(Broken::json& config) override;

	void AddGO(Broken::GameObject* go);
	void DeleteGO(Broken::GameObject* go);

	bool CleanUp() override;

	bool BuildNavMesh();
	bool BuildSoloNavMesh(const InputGeom* m_geom);
	bool BuildTiledNavMesh(const InputGeom* m_geom);
	void BuildTile(const InputGeom* m_geom, const int tx, const int ty, unsigned char** data, int* datasize);

	//Event handling
	static void ONGameObjectAdded(const Broken::Event& e);
	static void ONSceneUnloaded(const Broken::Event& e);
	static void ONGameObjectDeleted(const Broken::Event& e);

private:
	void MarkOBBArea(const math::OBB& obb, unsigned char areaId, rcCompactHeightfield& chf);

public:
	bool filterLowHangingObstacles = true;
	bool filterLedgeSpans = true;
	bool filterWalkableLowHeightSpans = true;

private:
	std::vector<Broken::GameObject*> NavigationGameObjects;

	//Recast variables
	rcConfig m_cfg;

};

#endif //__MODULERECAST_H__

