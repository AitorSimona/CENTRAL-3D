#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"

namespace Broken
{
	class GameObject;
}

class PanelInspector : public Panel
{
public:

	PanelInspector(char* name);
	~PanelInspector();

	bool Draw();

	bool Startup = true;

private:
	void CreateGameObjectNode(Broken::GameObject* Selected) const;
};

#endif
