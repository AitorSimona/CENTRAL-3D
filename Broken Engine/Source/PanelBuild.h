#ifndef __PANELBUILD_H__
#define __PANELBUILD_H__
#pragma once

#include "BrokenEngine.h"
#include <string>
#include <vector>

class BrokenEngine::GameObject;

class PanelBuild : public BrokenEngine::Panel {
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
	std::vector<BrokenEngine::GameObject*> cameras;
	std::string buildName;
	BrokenEngine::GameObject* selectedCamera;
	std::string scenePath;
	//std::string cameraName;

};

#endif