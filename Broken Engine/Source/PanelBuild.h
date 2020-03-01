#ifndef __PANELBUILD_H__
#define __PANELBUILD_H__
#pragma once

#include "Panel.h"
#include <string>
#include <vector>

class GameObject;

class PanelBuild : public Panel {

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
	unsigned int sceneUUID;
	unsigned int cameraGOUUID;

};

#endif