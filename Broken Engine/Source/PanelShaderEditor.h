#ifndef __PANEL_SHADER_EDITOR_H__
#define __PANEL_SHADER_EDITOR_H__

#include "BrokenEngine.h"
#include <vector>

class PanelShaderEditor : public Broken::Panel
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
