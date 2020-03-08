#ifndef __GAMEAPPLICATION_H__
#define __GAMEAPPLICATION_H__
#pragma once

#include "BrokenEngine.h"

class ModuleGameManager;

class GameApplication : public Broken::Application {
public:
	GameApplication();
	~GameApplication();

public:
	ModuleGameManager* game_manager;
};

extern GameApplication* gameApp;
#endif

