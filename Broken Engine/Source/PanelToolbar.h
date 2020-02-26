#ifndef __PANEL_TOOLBAR_H__
#define __PANEL_TOOLBAR_H__

#include "BrokenEngine.h"

class PanelToolbar : public BrokenEngine::Panel
{
public:

	PanelToolbar(char* name);
	~PanelToolbar();

	bool Draw();
};

#endif
