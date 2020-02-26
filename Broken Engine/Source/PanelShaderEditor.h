#ifndef __PANEL_SHADER_EDITOR_H__
#define __PANEL_SHADER_EDITOR_H__

#include "BrokenEngine.h"
#include <vector>

class PanelShaderEditor : public BrokenEngine::Panel
{
public:

	PanelShaderEditor(char* name);
	~PanelShaderEditor();

	bool Draw();
	void DisplayAndUpdateUniforms(BrokenEngine::ResourceMaterial* resource_mat);

private:
	BrokenEngine::ResourceShader* currentShader = nullptr;
};

#endif
