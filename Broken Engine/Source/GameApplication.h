#ifndef __GAMEAPPLICATION_H__
#define __GAMEAPPLICATION_H__
#pragma once

#include "Application.h"

class ModuleGameManager;

typedef unsigned int uint;

class GameApplication : public Broken::Application {
public:
	GameApplication();
	~GameApplication();

public:
	ModuleGameManager* game_manager;
};

extern GameApplication* GameApp;
#endif

