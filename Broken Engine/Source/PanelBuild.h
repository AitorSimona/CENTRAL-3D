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
	// @Improvement Sergi: this should not really use shared_ptrs if it can be avoided
	void copyAllFolderMT(std::shared_ptr<std::string> path, std::shared_ptr<std::string> build);

private:
	std::vector<Broken::GameObject*> cameras;
	std::string buildName;
	Broken::GameObject* selectedCamera;
	std::string sceneName;
	//std::string cameraName;

};

#endif