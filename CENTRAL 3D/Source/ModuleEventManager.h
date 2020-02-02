#ifndef __MODULE_EVENT_MANAGER_H__
#define __MODULE_EVENT_MANAGER_H__

#include "Module.h"

class GameObject;

struct Event
{
	enum class EventType
	{
		GameObject_destroyed,
		Window_resize,
		File_dropped,
		invalid
	} type;	

	union
	{
		GameObject* go = nullptr;
	};

	Event(EventType type) : type(type) {}

};

class ModuleEventManager : public Module
{
public:

	// --- Basic ---
	ModuleEventManager(bool start_enabled = true);
	~ModuleEventManager();

	bool Init(json file);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();
};

#endif