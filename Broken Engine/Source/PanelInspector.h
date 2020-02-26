#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "BrokenEngine.h"

class PanelInspector : public BrokenEngine::Panel
{
public:

	PanelInspector(char* name);
	~PanelInspector();

	bool Draw();

	bool Startup = true;

private:
	void CreateGameObjectNode(BrokenEngine::GameObject& Selected) const;
};

#endif
