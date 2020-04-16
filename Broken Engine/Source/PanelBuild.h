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
	void createFileFolders(const std::vector<std::string>& files);
	inline void getAllFolders(const std::string& file, std::vector<std::string>& outDirs);
	void copyAllFolder(const char* path, const char* build);
	void copyFile(const char* path, const char* build);

private:
	std::vector<Broken::GameObject*> cameras;
	std::string buildName;
	Broken::GameObject* selectedCamera;
	std::string sceneName;
	//std::string cameraName;

};

#endif