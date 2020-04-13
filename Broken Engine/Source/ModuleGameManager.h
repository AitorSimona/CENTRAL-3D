#ifndef __MODULEGAMEMANAGER_H__
#define __MODULEGAMEMANAGER_H__
#pragma once

#include "Module.h"

class ModuleGameManager : public Broken::Module {
public:
	ModuleGameManager(bool start_enabled = true);
	~ModuleGameManager();

	bool Init(Broken::json& config) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
};

#endif
