#ifndef __MODULEDETOUR_H__
#define __MODULEDETOUR_H__
#pragma once
#include "Module.h"
#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
#define BE_DETOUR_TOTAL_AREAS 31
class BROKEN_API ModuleDetour : public Module {
public:
	ModuleDetour(bool start_enabled = true);
	~ModuleDetour();

	bool Init(json& config) override;

	void setDefaultValues();

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

};

BE_END_NAMESPACE
#endif
