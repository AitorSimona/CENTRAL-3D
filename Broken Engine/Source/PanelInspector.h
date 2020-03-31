#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "BrokenEngine.h"


class PanelInspector : public Broken::Panel
{
public:

	PanelInspector(char* name);
	~PanelInspector();

	bool Draw();

	bool Startup = true;

private:
	void CreateGameObjectNode(Broken::GameObject& Selected) const;
};

#endif
