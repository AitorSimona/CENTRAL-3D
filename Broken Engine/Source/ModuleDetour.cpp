#include "ModuleDetour.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"

using namespace Broken;
ModuleDetour::ModuleDetour(bool start_enabled) : Module(start_enabled){
	name = "Detour";
}

ModuleDetour::~ModuleDetour() {
}

bool ModuleDetour::Init(json& config) {

	for (int i = 0; i < BE_DETOUR_TOTAL_AREAS; ++i) {
		sprintf_s(areaNames[i], "");
		areaCosts[i] = 1;
	}

	//Inititalize names
	sprintf_s(areaNames[0], "Walkable");
	sprintf_s(areaNames[1], "Not Walkable");
	sprintf_s(areaNames[2], "Jump");

	//Change value of Jump to 2 by default
	areaCosts[2] = 2;

	agentRadius = 0.5f;
	agentHeight = 2.0f;
	maxSlope = 45.0f;
	stepHeight = 0.4f;

	m_navQuery = dtAllocNavMeshQuery();

	return true;
}

bool ModuleDetour::createNavMesh(dtNavMeshCreateParams* params) {

	ENGINE_AND_SYSTEM_CONSOLE_LOG("Starting NavMesh build");
	unsigned char* navData = 0;
	int navDataSize = 0;

	dtFreeNavMesh(m_navMesh);
	m_navMesh = nullptr;

	if (!dtCreateNavMeshData(params, &navData, &navDataSize)) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not build Detour navmesh.");
		return false;
	}

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh) {
		dtFree(navData);
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not create Detour navmesh");
		return false;
	}

	dtStatus status;

	status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status)) {
		dtFree(navData);
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not init Detour navmesh");
		return false;
	}

	status = m_navQuery->init(m_navMesh, 2048);
	if (dtStatusFailed(status)) {
		ENGINE_AND_SYSTEM_CONSOLE_LOG("Could not init Detour navmesh query");
		return false;
	}

	//MYTODO Create resource

	return true;
}

bool ModuleDetour::CleanUp() {
	dtFreeNavMesh(m_navMesh);
	m_navMesh = nullptr;

	dtFreeNavMeshQuery(m_navQuery);
	m_navQuery = nullptr;

	return true;
}

void ModuleDetour::setDefaultValues() {
	for (int i = 3; i < BE_DETOUR_TOTAL_AREAS; ++i) {
		sprintf_s(areaNames[i], "");
		areaCosts[i] = 1;
	}

	//Inititalize names
	sprintf_s(areaNames[0], "Walkable");
	sprintf_s(areaNames[1], "Not Walkable");
	sprintf_s(areaNames[2], "Jump");

	//Change value of Jump to 2 by default
	areaCosts[2] = 2;

	agentRadius = 0.5f;
	agentHeight = 2.0f;
	maxSlope = 45.0f;
	stepHeight = 0.4f;
}
