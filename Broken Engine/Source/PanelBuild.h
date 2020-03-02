#ifndef __PANELBUILD_H__
#define __PANELBUILD_H__
#pragma once

#include "Panel.h"
#include <string>
#include <vector>
#include "Globals.h"
#include "JSONLoader.h"

class GameObject;

class PanelBuild : public Panel {
public:
	PanelBuild(char* name);
	~PanelBuild();

	bool Draw();
	inline void OnOff() override;
	inline void SetOnOff(bool set) override;

private:
	void findCameras();
	void makeBuild();
	void copyAllFolderMT(const char* path);

private:
	std::vector<GameObject*> cameras;
	std::string buildName;
	GameObject* selectedCamera;
	std::string scenePath;
	//std::string cameraName;

};

#endif