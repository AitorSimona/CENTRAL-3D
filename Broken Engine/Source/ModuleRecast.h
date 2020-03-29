#ifndef __MODULERECAST_H__
#define __MODULERECAST_H__
#pragma once
#include "Module.h"
#include "BrokenEngine.h"
#include "Recast.h"

/// The maximum number of vertices per navigation polygon.
/// @ingroup detour
static const int DT_VERTS_PER_POLYGON = 6;

class ModuleRecast : public Broken::Module {
public:
	ModuleRecast(bool start_enabled = true);
	~ModuleRecast();

	bool Init(Broken::json& config) override;

	update_status PostUpdate(float dt) override;

	void AddGO(Broken::GameObject* go);
	void DeleteGO(Broken::GameObject* go);

	bool CleanUp() override;

	bool BuildNavMesh();

	//Event handling
	static void ONGameObjectAdded(const Broken::Event& e);
	static void ONSceneUnloaded(const Broken::Event& e);
	static void ONGameObjectDeleted(const Broken::Event& e);

public:
	bool filterLowHangingObstacles = true;
	bool filterLedgeSpans = true;
	bool filterWalkableLowHeightSpans = true;

private:
	std::vector<Broken::GameObject*> NavigationGameObjects;

	//Recast variables
	unsigned char* m_triareas = nullptr;
	rcHeightfield* m_solid = nullptr;
	rcCompactHeightfield* m_chf = nullptr;
	rcContourSet* m_cset = nullptr;
	rcPolyMesh* m_pmesh = nullptr;
	rcConfig m_cfg;
	rcPolyMeshDetail* m_dmesh = nullptr;
	rcContext* m_ctx;

};

#endif //__MODULERECAST_H__

