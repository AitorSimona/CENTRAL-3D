#ifndef __PANEL_INSPECTOR_H__
#define __PANEL_INSPECTOR_H__

#include "Panel.h"

class GameObject;

class PanelInspector : public Panel
{
public:

	PanelInspector(char* name);
	~PanelInspector();

	bool Draw();

	bool Startup = true;

private:
	inline void CreateGameObjectNode(GameObject& Selected) const;
	inline void CreateTransformNode(GameObject& Selected) const;
	inline void CreateMeshNode(GameObject& Selected) const;
	inline void CreateRendererNode(GameObject& Selected) const;
	inline void CreateMaterialNode(GameObject& Selected) const;
	inline void CreateCameraNode(GameObject& Selected) const;

};

#endif
