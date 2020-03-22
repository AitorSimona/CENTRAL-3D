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

	bool navigationStatic = false;
	bool popupNavigationFlag = false;
	uint popupArea = 0;
	bool openPopup = false;
	bool affectChilds = false;
	bool doChange = false;


};

#endif

