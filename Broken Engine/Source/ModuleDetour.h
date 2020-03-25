#ifndef __MODULEDETOUR_H__
#define __MODULEDETOUR_H__
#pragma once
#include "Module.h"
#include "BrokenCore.h"

class dtNavMeshCreateParams;
class dtNavMesh;
class dtNavMeshQuery;

BE_BEGIN_NAMESPACE
#define BE_DETOUR_TOTAL_AREAS 31
enum PolyFlags {
	POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	POLYFLAGS_SWIM = 0x02,      // Ability to swim (water).
	POLYFLAGS_DOOR = 0x04,      // Ability to move through doors.
	POLYFLAGS_JUMP = 0x08,      // Ability to jump.
	POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	POLYFLAGS_ALL = 0xffff      // All abilities.
};

class ResourceNavMesh;

class BROKEN_API ModuleDetour : public Module {
public:
	ModuleDetour(bool start_enabled = true);
	~ModuleDetour();

	bool Init(json& config) override;
	bool CleanUp() override;

	void Draw() const;
	void setDebugDraw(bool state);

	bool createNavMesh(dtNavMeshCreateParams* params);
	void loadNavMeshFile(uint UID);

	void setDefaultValues();
	void setDefaultBakeValues();
	const ResourceNavMesh* getNavMeshResource() const;

public:
	float agentRadius = 0.5f;
	float agentHeight = 2.0f;
	float maxSlope = 45.0f;
	float stepHeight = 0.4f;
	float voxelSize = 0.15f;
	float voxelHeight = 0.2f;

	float regionMinSize = 8;
	float regionMergeSize = 20;
	float edgeMaxLen = 12.0f;
	float edgeMaxError = 1.3f;
	float vertsPerPoly = 6.0f;
	float detailSampleDist = 6.0f;
	float detailSampleMaxError = 1.0f;

	char areaNames[BE_DETOUR_TOTAL_AREAS][100];
	int areaCosts[BE_DETOUR_TOTAL_AREAS];

private:
	bool debugDraw = false;
	class DebugDrawGL* m_dd = nullptr;
	ResourceNavMesh* navMeshResource = nullptr;
	dtNavMeshQuery* m_navQuery = nullptr;


};

BE_END_NAMESPACE
#endif
