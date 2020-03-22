#ifndef __PANELNAVIGATION_H__
#define __PANELNAVIGATION_H__
#pragma once


#include "Panel.h"
#include "BrokenEngine.h"
class PanelNavigation : public Broken::Panel {
public:

	PanelNavigation(char* name);
	~PanelNavigation();

	bool Draw();

private:
	float agentRadius = 0.5f;
	float agentHeight = 2.0f;
	float maxSlope = 45.0f;
	float stepHeight = 0.4f;


};

#endif

