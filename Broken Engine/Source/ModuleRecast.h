#ifndef __MODULERECAST_H__
#define __MODULERECAST_H__
#pragma once
#include "Module.h"
#include "BrokenEngine.h"

//namespace Broken
//{
//	struct Event;
//}

class ModuleRecast : public Broken::Module {
public:
	ModuleRecast(bool start_enabled = true);
	~ModuleRecast();

	bool Init(Broken::json& config) override;

	void AddGO(Broken::GameObject* go);
	void DeleteGO(Broken::GameObject* go);

	static void ONGameObjectAdded(const Broken::Event& e);

private:
	std::vector<Broken::GameObject*> NavigationGameObjects;

};

#endif //__MODULERECAST_H__

