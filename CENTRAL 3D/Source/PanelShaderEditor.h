#ifndef __PANEL_SHADER_EDITOR_H__
#define __PANEL_SHADER_EDITOR_H__

#include "Panel.h"
#include <vector>

class ResourceShader;
class ResourceMaterial;

class PanelShaderEditor : public Panel
{
public:

	PanelShaderEditor(char* name);
	~PanelShaderEditor();

	bool Draw();
	void DisplayAndUpdateUniforms(ResourceMaterial* resource_mat);

private:
	ResourceShader* currentShader = nullptr;
};

#endif
