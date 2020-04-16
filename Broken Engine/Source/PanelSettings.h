#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__

#define FPS_TRACKER_SIZE 100


#include <vector>
#include "Panel.h"


class PanelSettings : public Panel
{
public:

	PanelSettings(char* name);
	~PanelSettings();

	void AddFPS(const float& fps, const float& ms);
	bool Draw();

private:

	inline void ApplicationNode() const;
	inline void WindowNode() const;
	inline void InputNode() const;
	inline void RendererNode() const;
	inline void HardwareNode() const;
	inline void SoftwareNode() const;
	inline void RAMMemoryNode() const;
	inline void LibrariesNode() const;
	inline void EngineCameraNode() const;

private:
	std::vector<float> FPS_Tracker;
	std::vector<float> MS_Tracker;
};

#endif