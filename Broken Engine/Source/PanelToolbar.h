#ifndef __PANEL_TOOLBAR_H__
#define __PANEL_TOOLBAR_H__

#include "Panel.h"

class PanelToolbar : public Panel
{
public:

	PanelToolbar(char* name);
	~PanelToolbar();

	bool Draw();

};

#endif
