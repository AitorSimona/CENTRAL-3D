#ifndef __MODULE_EVENT_MANAGER_H__
#define __MODULE_EVENT_MANAGER_H__

#include "Module.h"
#include "BrokenCore.h"

#define MAX_EVENTS 1000
#define EVENT_TYPES 7

typedef void (*Function)(const Broken::Event& e);

BE_BEGIN_NAMESPACE

class GameObject;
class Resource;

struct BROKEN_API Event {
	enum class EventType {
		GameObject_destroyed,
		GameObject_selected,
		GameObject_loaded,
		Scene_unloaded,
		Resource_selected,
		Resource_destroyed,
		Window_resize,
		File_dropped,
		invalid
	} type = EventType::invalid;

	union {
		GameObject* go = nullptr;
		Resource* resource;
		uint uid; // For overwrite case
	};

	Event(EventType type) : type(type) {}
	Event() {}
};

struct BROKEN_API Listeners {
	std::vector<Function> listeners;
};

class BROKEN_API ModuleEventManager : public Module {
public:

	// --- Basic ---
	ModuleEventManager(bool start_enabled = true);
	~ModuleEventManager();

	bool Init(json& file) override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;

	void PushEvent(Event& new_event);
	void AddListener(Event::EventType type, Function callback);
	void RemoveListener(Event::EventType type, Function callback);

private:
	Event events[MAX_EVENTS];
	Listeners listeners[EVENT_TYPES];

	uint head;
	uint tail;
};

BE_END_NAMESPACE
#endif