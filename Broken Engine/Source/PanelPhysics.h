#ifndef __PANEL_PHYSICS_H__
#define __PANEL_PHYSICS_H__

#include "Panel.h"

class PanelPhysics : public Panel
{
public:

	PanelPhysics(char* name);
	~PanelPhysics();

	bool Draw();

	void CreateLayerFilterGrid();

	void CreateLayerList();

};
#endif