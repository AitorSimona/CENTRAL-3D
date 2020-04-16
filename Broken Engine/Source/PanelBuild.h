#ifndef __PANELBUILD_H__
#define __PANELBUILD_H__
#pragma once

#include <string>
#include <vector>
#include <memory>
#include "Panel.h"

namespace Broken
{
	class GameObject;
}

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
	void copyFile(const char* path, const char* buildFolder);
	void createFoldersAndRetrieveFiles(const char* path, const char* newPath, std::vector<std::string>& outFiles);


private:
	std::vector<Broken::GameObject*> cameras;
	std::string buildName;
	Broken::GameObject* selectedCamera;
	std::string sceneName;
	//std::string cameraName;

};

#endif