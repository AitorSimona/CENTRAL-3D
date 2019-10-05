#ifndef __PANEL_CONSOLE_H__
#define __PANEL_CONSOLE_H__

#include "Panel.h"

class PanelConsole : public Panel
{
public:

	PanelConsole(char* name);
	~PanelConsole();

	bool Draw();

private:

	void Clear();

};

#endif

