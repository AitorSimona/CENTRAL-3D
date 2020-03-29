#include "ModuleRecast.h"
#include "EngineApplication.h"
#include "InputGeometry.h"
#include "mmgr/mmgr.h"
#include "DetourNavMeshBuilder.h"
#include "ResourceMesh.h"
#include "ModuleRenderer3D.h"
#include "ResourceMaterial.h"
#include "RecastDebugDraw.h"


ModuleRecast::ModuleRecast(bool start_enabled) : Broken::Module(start_enabled) {
	name = "Recast";
	m_ctx = new rcContext();

}

ModuleRecast::~ModuleRecast() {
	delete m_ctx;
}

bool ModuleRecast::Init(Broken::json& config) {
	EngineApp->event_manager->AddListener(Broken::Event::EventType::GameObject_loaded, ONGameObjectAdded);
	EngineApp->event_manager->AddListener(Broken::Event::EventType::Scene_unloaded, ONSceneUnloaded);
	EngineApp->event_manager->AddListener(Broken::Event::EventType::GameObject_destroyed, ONGameObjectDeleted);
	return true;
}

update_status ModuleRecast::PostUpdate(float dt) {
	if (m_pmesh != nullptr) {
		duDebugDrawPolyMesh((duDebugDraw*)EngineApp->detour->m_dd, *m_pmesh);
		duDebugDrawPolyMeshDetail((duDebugDraw*)EngineApp->detour->m_dd, * m_dmesh);
	}

	return UPDATE_CONTINUE;
}

void ModuleRecast::AddGO(Broken::GameObject* go) {
	if (go->navigationStatic)
		NavigationGameObjects.push_back(go);
}

void ModuleRecast::DeleteGO(Broken::GameObject* go) {
	for (std::vector<Broken::GameObject*>::iterator it = NavigationGameObjects.begin(); it != NavigationGameObjects.end(); ++it) {
		if (*it == go) {
			NavigationGameObjects.erase(it);
			break;
		}
	}
}

bool ModuleRecast::CleanUp() {

	if (m_triareas != nullptr)
		delete[] m_triareas;
	m_triareas = nullptr;
	
	if (m_solid != nullptr)
		rcFreeHeightField(m_solid);
	m_solid = nullptr;

	if (m_chf != nullptr)
		rcFreeCompactHeightfield(m_chf);
	m_chf = nullptr;

	if (m_cset != nullptr)
		rcFreeContourSet(m_cset);
	m_cset = nullptr;

	if (m_pmesh != nullptr)
		rcFreePolyMesh(m_pmesh);
	m_pmesh = nullptr;

	if (m_dmesh != nullptr)
		rcFreePolyMeshDetail(m_dmesh);
	m_dmesh = nullptr;
	return true;
}

bool ModuleRecast::BuildNavMesh() {
	EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Starting NavMesh build");

	if (NavigationGameObjects.size() == 0) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: No input mesh");
		return false;
	}

	CleanUp();

	InputGeom* m_geom = new InputGeom(NavigationGameObjects);
	//
	// Step 1. Initialize build config.
	//
	// Reset build times gathering.
	m_ctx->resetTimers();

	// Start the build process.
	m_ctx->startTimer(RC_TIMER_TOTAL);

	// Init build configuration from GUI
	memset(&m_cfg, 0, sizeof(m_cfg));
	m_cfg.cs = EngineApp->detour->voxelSize;
	m_cfg.ch = EngineApp->detour->voxelHeight;
	m_cfg.walkableSlopeAngle = EngineApp->detour->maxSlope;
	m_cfg.walkableHeight = (int)ceilf(EngineApp->detour->agentHeight / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(EngineApp->detour->stepHeight / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(EngineApp->detour->agentRadius / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(EngineApp->detour->edgeMaxLen / m_cfg.cs);
	m_cfg.maxSimplificationError = EngineApp->detour->edgeMaxError;
	m_cfg.minRegionArea = (int)rcSqr(EngineApp->detour->regionMinSize);		// Note: area = size*size
	m_cfg.mergeRegionArea = (int)rcSqr(EngineApp->detour->regionMergeSize);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int)EngineApp->detour->vertsPerPoly;
	m_cfg.detailSampleDist = EngineApp->detour->detailSampleDist < 0.9f ? 0 : m_cfg.cs * EngineApp->detour->detailSampleDist;
	m_cfg.detailSampleMaxError = m_cfg.cs * EngineApp->detour->detailSampleMaxError;

	// Set the area where the navigation will be build.
	// Here the bounds of the input mesh are used, but the
	// area could be specified by an user defined box, etc.
	rcVcopy(m_cfg.bmin, m_geom->getMeshBoundsMin());
	rcVcopy(m_cfg.bmax, m_geom->getMeshBoundsMax());
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);

	int nverts = m_geom->getVertCount();
	int ntris = m_geom->getTriCount();

	EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Recast: Building navigation:");
	EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Recast: - %d x %d cells", m_cfg.width, m_cfg.height);
	EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("Recast: - %.1fK verts, %.1fK tris", nverts / 1000.0f, ntris / 1000.0f);


	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[m_geom->getMaxTris()];
	if (!m_triareas) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'm_triareas' (%d).", m_geom->getMaxTris());
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	for (int i = 0; i < m_geom->getMeshes().size(); ++i) {
		memset(m_triareas, 0, m_geom->getMaxTris() * sizeof(unsigned char));
		rcMarkWalkableTriangles(m_ctx, m_cfg.walkableSlopeAngle, m_geom->getMeshes()[i].verts, m_geom->getMeshes()[i].nverts, m_geom->getMeshes()[i].tris, m_geom->getMeshes()[i].ntris, m_triareas);
		rcRasterizeTriangles(m_ctx, m_geom->getMeshes()[i].verts, m_geom->getMeshes()[i].nverts, m_geom->getMeshes()[i].tris, m_triareas, m_geom->getMeshes()[i].ntris, *m_solid, m_cfg.walkableClimb);
	}

	delete[] m_triareas;
	m_triareas = nullptr;

	//
	// Step 3. Filter walkables surfaces.
	//

	// Once all geoemtry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	if (filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg.walkableClimb, *m_solid);
	if (filterLedgeSpans)
		rcFilterLedgeSpans(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	if (filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(m_ctx, m_cfg.walkableHeight, *m_solid);

	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	m_chf = rcAllocCompactHeightfield();
	if (!m_chf) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'chf'.");
		return false;
	}
	if (!rcBuildCompactHeightfield(m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build compact data.");
		return false;
	}

	rcFreeHeightField(m_solid);
	m_solid = nullptr;

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(m_ctx, m_cfg.walkableRadius, *m_chf)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	const ConvexVolume* vols = m_geom->getConvexVolumes();
	for (int i  = 0; i < m_geom->getConvexVolumeCount(); ++i)
	   rcMarkConvexPolyArea(m_ctx, vols[i].verts, vols[i].nverts, vols[i].hmin, vols[i].hmax, (unsigned char)vols[i].area, *m_chf);

	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// Using Watershed partitioning
	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	if (!rcBuildDistanceField(m_ctx, *m_chf)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build distance field.");
		return false;
	}

	// Partition the walkable surface into simple regions without holes.
	if (!rcBuildRegions(m_ctx, *m_chf, m_cfg.borderSize, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build regions.");
		return false;
	}

	//
	// Step 5. Trace and simplify region contours.
	//

	// Create contours.
	m_cset = rcAllocContourSet();
	if (!m_cset) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'cset'.");
		return false;
	}
	if (!rcBuildContours(m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not create contours.");
		return false;
	}

	//
	// Step 6. Build polygons mesh from contours.
	//

	// Build polygon navmesh from the contours.
	m_pmesh = rcAllocPolyMesh();
	if (!m_pmesh) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'pmesh'.");
		return false;
	}
	if (!rcBuildPolyMesh(m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not triangulate contours.");
		return false;
	}

	//
	// Step 7. Create detail mesh which allows to access approximate height on each polygon.
	//

	m_dmesh = rcAllocPolyMeshDetail();
	if (!m_dmesh) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'pmdtl'.");
		return false;
	}

	if (!rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build detail mesh.");
		return false;
	}

	rcFreeCompactHeightfield(m_chf);
	m_chf = nullptr;
	rcFreeContourSet(m_cset);
	m_cset = nullptr;


	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	if (m_cfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON) {
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i) {
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA) 
				m_pmesh->flags[i] = Broken::PolyFlags::POLYFLAGS_WALK;
		}

		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;

		//No offmesh connections yet
		params.offMeshConCount = m_geom->getOffMeshConnectionCount();
		params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		params.offMeshConUserID = m_geom->getOffMeshConnectionId();


		params.walkableHeight = EngineApp->detour->agentHeight;
		params.walkableRadius = EngineApp->detour->agentRadius;
		params.walkableClimb = EngineApp->detour->stepHeight;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg.cs;
		params.ch = m_cfg.ch;
		params.buildBvTree = true;

		EngineApp->detour->createNavMesh(&params);
	}

	delete m_geom;
	return true;

}

void ModuleRecast::ONGameObjectAdded(const Broken::Event& e) {
	if (e.go->navigationStatic)
		EngineApp->recast->NavigationGameObjects.push_back(e.go);
}

void ModuleRecast::ONSceneUnloaded(const Broken::Event& e) {
	//If the scene is unloaded all our GOs are not valid anymore so we clear it.
	EngineApp->recast->NavigationGameObjects.clear();
}

void ModuleRecast::ONGameObjectDeleted(const Broken::Event& e) {
	if (e.go->navigationStatic) {
		for (std::vector<Broken::GameObject*>::iterator it = EngineApp->recast->NavigationGameObjects.begin(); it != EngineApp->recast->NavigationGameObjects.end(); ++it) {
			if (*it == e.go) {
				EngineApp->recast->NavigationGameObjects.erase(it);
				break;
			}
		}
	}
}
