#ifndef __PANEL_SHADER_EDITOR_H__
#define __PANEL_SHADER_EDITOR_H__

#include "Panel.h"

namespace Broken
{
	class ResourceMaterial;
	class ResourceShader;
}

class PanelShaderEditor : public Panel
{
public:

	PanelShaderEditor(char* name);
	~PanelShaderEditor();

	bool Draw();
	void DisplayAndUpdateUniforms(Broken::ResourceMaterial* resource_mat);

private:
	Broken::ResourceShader* currentShader = nullptr;
};

#endif
