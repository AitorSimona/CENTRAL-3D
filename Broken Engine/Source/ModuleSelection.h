#ifndef __SELECTION_H__
#define __SELECTION_H__
#include "Module.h"

BE_BEGIN_NAMESPACE


class BROKEN_API ModuleSelection :	public Module
{
public:
	ModuleSelection(bool start_enabled = true);
	~ModuleSelection();
	bool Init(json& file);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
};

BE_END_NAMESPACE
#endif