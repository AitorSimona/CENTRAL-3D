#ifndef __PANELBUILD_H__
#define __PANELBUILD_H__
#pragma once

#include "BrokenEngine.h"
#include <string>
#include <vector>

class Broken::GameObject;

class PanelBuild : public Broken::Panel {
public:
	PanelBuild(char* name);
	~PanelBuild();

	bool Draw();
	inline void OnOff() override;
	inline void SetOnOff(bool set) override;

private:
	void findCameras();
	void makeBuild();
	void copyAllFolderMT(std::shared_ptr<std::string> path, std::shared_ptr<std::string> build);

private:
	std::vector<Broken::GameObject*> cameras;
	std::string buildName;
	Broken::GameObject* selectedCamera;
	std::string sceneName;
	//std::string cameraName;

};

#endif