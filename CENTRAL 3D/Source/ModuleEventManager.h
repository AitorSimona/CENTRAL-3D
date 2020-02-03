#ifndef __MODULE_EVENT_MANAGER_H__
#define __MODULE_EVENT_MANAGER_H__

#include "Module.h"
#include "Globals.h"

class GameObject;

#define MAX_EVENTS 25
//#define EVENT_TYPES 4

struct Event
{
	enum class EventType
	{
		GameObject_destroyed,
		Window_resize,
		File_dropped,
		invalid
	} type = EventType::invalid;	

	union
	{
		GameObject* go = nullptr;
	};

	Event(EventType type) : type(type) {}
	Event() {}
};

class ModuleEventManager : public Module
{
public:

	// --- Basic ---
	ModuleEventManager(bool start_enabled = true);
	~ModuleEventManager();

	bool Init(json file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	bool CleanUp() override;

	void PushEvent(Event& new_event);

private:
	Event events[MAX_EVENTS];

	uint head;
	uint tail;
};

#endif