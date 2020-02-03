#include "ModuleEventManager.h"

#include "mmgr/mmgr.h"

ModuleEventManager::ModuleEventManager(bool start_enabled)
{
	head = 0;
	tail = 0;
}

ModuleEventManager::~ModuleEventManager()
{
}

bool ModuleEventManager::Init(json file)
{


	return true;
}

bool ModuleEventManager::Start()
{
	return true;
}

update_status ModuleEventManager::PreUpdate(float dt)
{
	while (head != tail)
	{
		// Process events
		static_assert(static_cast<int>(Event::EventType::invalid) == 3, "Event Processor Switch needs to be updated");

		switch (events[head].type)
		{
			case Event::EventType::GameObject_destroyed:

			break;

			case Event::EventType::Window_resize:

			break;

			case Event::EventType::File_dropped:

			break;

			case Event::EventType::invalid:

			break;

			default:

			break;
		}

		head++;
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEventManager::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleEventManager::CleanUp()
{
	return true;
}

void ModuleEventManager::PushEvent(Event& new_event)
{
	// --- Add given event to the end of the list ---
	events[tail] = new_event;
	tail = (tail + 1) % MAX_EVENTS;
}
