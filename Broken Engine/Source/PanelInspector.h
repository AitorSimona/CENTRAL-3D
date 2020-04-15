#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "BrokenEngine.h"


class PanelInspector : public Broken::Panel
{
public:

	PanelInspector(char* name);
	~PanelInspector();

	bool Draw();

	void CreateComponentOptionsMenu(std::vector<Broken::Component*>::const_iterator& it);

	bool Startup = true;

private:
	void CreateGameObjectNode(Broken::GameObject& Selected) const;
};

#endif
