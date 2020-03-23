#include "ModuleRecast.h"
#include "EngineApplication.h"

ModuleRecast::ModuleRecast(bool start_enabled) : Broken::Module(start_enabled) {
	name = "Recast";
}

ModuleRecast::~ModuleRecast() {
}

bool ModuleRecast::Init(Broken::json& config) {
	EngineApp->event_manager->AddListener(Broken::Event::EventType::GameObject_loaded, ONGameObjectAdded);
	EngineApp->event_manager->AddListener(Broken::Event::EventType::Scene_unloaded, ONSceneUnloaded);
	m_ctx = rcContext(false);
	return true;
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
	if (NavigationGameObjects.size() == 0) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: No input mesh");
		return false;
	}

	CleanUp();

	float3 bmin;
	float3 bmax;
	std::vector<float> verts;
	int nverts = 0;
	std::vector<int> indices;
	int nindices = 0;

	std::vector<std::pair<Polyhedron, uint>> convex_volumes;

	Broken::ResourceMesh* r_mesh;
	Broken::ComponentTransform* comp_trans;
	for (std::vector<Broken::GameObject*>::const_iterator it = NavigationGameObjects.cbegin(); it != NavigationGameObjects.cend(); ++it) {
		r_mesh = (*it)->GetComponent<Broken::ComponentMesh>()->resource_mesh;
		comp_trans = (*it)->GetComponent<Broken::ComponentTransform>();

		// We add the indices plus the current vertices since we are combining all triangles
		nindices += r_mesh->IndicesSize;
		for (int i = 0; i < r_mesh->IndicesSize; ++i)
			indices.push_back(r_mesh->Indices[i] + nverts);

		nverts += r_mesh->VerticesSize;
		float4x4 transform = comp_trans->GetGlobalTransform();
		for (int i = 0; i < r_mesh->VerticesSize; ++i) {
			float t_vertex[3];
			ApplyTransform(r_mesh->vertices[i], transform, t_vertex);
			verts.push_back(t_vertex[0]);
			verts.push_back(t_vertex[1]);
			verts.push_back(t_vertex[2]);
		}

		const AABB &aabb = r_mesh->aabb;

		if (it == NavigationGameObjects.cbegin()) {
			bmin = aabb.minPoint;
			bmax = aabb.maxPoint;
		}
		else {
			bmin = math::Min(bmin, aabb.minPoint);
			bmax = math::Max(bmax, aabb.maxPoint);
		}

		//Create convex hull for later flagging polys
		r_mesh->CreateOBB();
		OBB t_obb = r_mesh->obb;
		t_obb.Transform(transform);
		
		std::pair<Polyhedron, uint> convex_volume;
		convex_volume.first = t_obb.ToPolyhedron();
		convex_volume.second = (*it)->navigationArea;

		if (convex_volume.second == 0)
			convex_volume.second = RC_WALKABLE_AREA;
		else if (convex_volume.second == 1)
			convex_volume.second = RC_NULL_AREA;

		convex_volumes.push_back(convex_volume);
	}

	nindices /= 3;
	//
	// Step 1. Initialize build config.
	//

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
	rcVcopy(m_cfg.bmin, bmin.ptr());
	rcVcopy(m_cfg.bmax, bmax.ptr());
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);


	//
	// Step 2. Rasterize input polygon soup.
	//

	// Allocate voxel heightfield where we rasterize our input data to.
	m_solid = rcAllocHeightfield();
	if (!m_solid) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'solid'.");
		return false;
	}
	if (!rcCreateHeightfield(&m_ctx, *m_solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not create solid heightfield.");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	m_triareas = new unsigned char[nindices];
	if (!m_triareas) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Out of memory 'm_triareas' (%d).", nindices);
		return false;
	}

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(m_triareas, 0, nindices * sizeof(unsigned char));
	rcMarkWalkableTriangles(&m_ctx, m_cfg.walkableSlopeAngle, verts.data(), nverts, indices.data(), nindices, m_triareas);
	if (!rcRasterizeTriangles(&m_ctx, verts.data(), nverts, indices.data(), m_triareas, nindices, *m_solid, m_cfg.walkableClimb)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not rasterize triangles.");
		return false;
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
		rcFilterLowHangingWalkableObstacles(&m_ctx, m_cfg.walkableClimb, *m_solid);
	if (filterLedgeSpans)
		rcFilterLedgeSpans(&m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid);
	if (filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(&m_ctx, m_cfg.walkableHeight, *m_solid);

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
	if (!rcBuildCompactHeightfield(&m_ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *m_solid, *m_chf)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build compact data.");
		return false;
	}

	rcFreeHeightField(m_solid);
	m_solid = nullptr;

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(&m_ctx, m_cfg.walkableRadius, *m_chf)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not erode.");
		return false;
	}

	// (Optional) Mark areas.
	for (std::vector<std::pair<Polyhedron, uint>>::iterator it = convex_volumes.begin(); it != convex_volumes.end(); ++it) {
		AABB polaabb = (*it).first.MinimalEnclosingAABB();
		vec* first_vec = (*it).first.VertexArrayPtr();
		rcMarkConvexPolyArea(&m_ctx, (*first_vec).ptr(), (*it).first.NumVertices(), polaabb.MinY(), polaabb.MaxY(),(*it).second, *m_chf);
	}

	// Partition the heightfield so that we can use simple algorithm later to triangulate the walkable areas.
	// Using Watershed partitioning
	// Prepare for region partitioning, by calculating distance field along the walkable surface.
	if (!rcBuildDistanceField(&m_ctx, *m_chf)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build distance field.");
		return false;
	}

	// Partition the walkable surface into simple regions without holes.
	if (!rcBuildRegions(&m_ctx, *m_chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build watershed regions.");
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
	if (!rcBuildContours(&m_ctx, *m_chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *m_cset)) {
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
	if (!rcBuildPolyMesh(&m_ctx, *m_cset, m_cfg.maxVertsPerPoly, *m_pmesh)) {
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

	if (!rcBuildPolyMeshDetail(&m_ctx, *m_pmesh, *m_chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *m_dmesh)) {
		EX_ENGINE_AND_SYSTEM_CONSOLE_LOG("RC_ERROR: buildNavigation: Could not build detail mesh.");
		return false;
	}

	rcFreeCompactHeightfield(m_chf);
	m_chf = 0;
	rcFreeContourSet(m_cset);
	m_cset = 0;


	// At this point the navigation mesh data is ready, you can access it from m_pmesh.
	// See duDebugDrawPolyMesh or dtCreateNavMeshData as examples how to access the data.

	//
	// (Optional) Step 8. Create Detour data from Recast poly mesh.
	//

	// Here we will call the Detour module

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

void ModuleRecast::ApplyTransform(const Broken::Vertex& vertex, const float4x4& transform, float ret[3]) {
	float4 intermid;
	intermid.x = vertex.position[0]; 
	intermid.y = vertex.position[1];
	intermid.z = vertex.position[2];
	intermid.w = 1;

	intermid = transform * intermid;
	ret[0] = intermid.x / intermid.w;
	ret[1] = intermid.y / intermid.w;
	ret[2] = intermid.z / intermid.w;
}
