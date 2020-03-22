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

	char areaNames[BE_DETOUR_TOTAL_AREAS][100];
	int areaCosts[BE_DETOUR_TOTAL_AREAS];

};

BE_END_NAMESPACE
#endif
