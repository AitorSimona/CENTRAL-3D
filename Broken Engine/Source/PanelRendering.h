#ifndef __PANEL_RENDERING_H__
#define __PANEL_RENDERING_H__

#include "BrokenEngine.h"

class PanelRendering : public Broken::Panel
{
public:

	PanelRendering(char* name);
	~PanelRendering();

	bool Draw();

private:

	float m_GammaCorretionValue = 1.0f;
};

#endif