#ifndef __PANEL_SHADER_EDITOR_H__
#define __PANEL_SHADER_EDITOR_H__

#include "Panel.h"

class ResourceShader;

class PanelShaderEditor : public Panel
{
public:

	PanelShaderEditor(char* name);
	~PanelShaderEditor();

	bool Draw();

	void HandleUniforms();


	ResourceShader* currentShader = nullptr;
};

#endif
