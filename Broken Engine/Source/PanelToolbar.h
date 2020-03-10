#ifndef __PANEL_TOOLBAR_H__
#define __PANEL_TOOLBAR_H__

#include "BrokenEngine.h"

class PanelToolbar : public Broken::Panel
{
public:

	PanelToolbar(char* name);
	~PanelToolbar();

	bool Draw();

private:
	std::string play_button;
	std::string pause_button;
};

#endif
