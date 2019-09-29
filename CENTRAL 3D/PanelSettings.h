#ifndef __PANEL_SETTINGS_H__
#define __PANEL_SETTINGS_H__

#define SCREEN_MIN_WIDTH 640
#define SCREEN_MIN_HEIGHT 480

#include "Panel.h"

class Module;

class PanelSettings : public Panel
{
public:

	PanelSettings(char* name);
	~PanelSettings();

	bool Draw();

	//void AddInput(const char* input);

private:

	void ApplicationNode() const;
	void WindowNode() const;
	void InputNode() const;
	void HardwareNode() const;
};

#endif

// App, Window, Input , Hardware