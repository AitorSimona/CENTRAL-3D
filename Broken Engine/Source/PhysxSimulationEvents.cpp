#include "PhysxSimulationEvents.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "GameObject.h"
#include "ComponentScript.h"
#include "ModuleScripting.h"

using namespace Broken;

PhysxSimulationEvents::PhysxSimulationEvents(ModulePhysics* callback)
{
	this->callback = callback;
}

void PhysxSimulationEvents::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	for (physx::PxU32 i = 0; i < nbPairs; ++i)
	{
		const physx::PxContactPair& cp = pairs[i];

		GameObject* go1 = nullptr;
		GameObject* go2 = nullptr;
		go1 = App->physics->actors[pairHeader.actors[0]];
		go2 = App->physics->actors[pairHeader.actors[1]];

		if (go1 && go2)
		{
			ComponentScript* script = go1->GetComponent<ComponentScript>();
			ComponentScript* script2 = go2->GetComponent<ComponentScript>();

			if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{		
				go1->collisions.at(ONCOLLISION_ENTER) = go2;
				go2->collisions.at(ONCOLLISION_ENTER) = go1;
				
				ScriptFunc function;
				function.name = "OnCollisionEnter";

				if(script)
					App->scripting->CallbackScriptFunction(script, function);
				if (script2)
					App->scripting->CallbackScriptFunction(script2, function);			
			}

			else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS)
			{
				go1->collisions.at(ONCOLLISION_STAY) = go2;
				go2->collisions.at(ONCOLLISION_STAY) = go1;

				ScriptFunc function;
				function.name = "OnCollisionStay";

				if (script)
					App->scripting->CallbackScriptFunction(script, function);
				if (script2)
					App->scripting->CallbackScriptFunction(script2, function);
			}

			else if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				go1->collisions.at(ONCOLLISION_EXIT) = go2;
				go2->collisions.at(ONCOLLISION_EXIT) = go1;

				ScriptFunc function;
				function.name = "OnCollisionExit";

				if (script)
					App->scripting->CallbackScriptFunction(script, function);
				if (script2)
					App->scripting->CallbackScriptFunction(script2, function);
			}
		}
	}
}

void PhysxSimulationEvents::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; ++i)
	{
		// ignore pairs when shapes have been deleted
		if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
			continue;

		GameObject* go1 = nullptr;
		GameObject* go2 = nullptr;
		go1 = App->physics->actors[pairs->triggerActor];
		go2 = App->physics->actors[pairs->otherActor];

		ComponentScript* script = go1->GetComponent<ComponentScript>();
		ComponentScript* script2 = go2->GetComponent<ComponentScript>();
		if (go1 != nullptr && go2 != nullptr) {
			if ((pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND))
			{
				go1->collisions.at(ONTRIGGER_ENTER) = go2;
				go2->collisions.at(ONTRIGGER_ENTER) = go1;

				ScriptFunc function;
				function.name = "OnTriggerEnter";

				if (script)
					App->scripting->CallbackScriptFunction(script, function);
				if (script2)
					App->scripting->CallbackScriptFunction(script2, function);
			}
			else if ((pairs[i].status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST))
			{
				go1->collisions.at(ONTRIGGER_EXIT) = go2;
				go2->collisions.at(ONTRIGGER_EXIT) = go1;

				ScriptFunc function;
				function.name = "OnTriggerExit";

				if (script)
					App->scripting->CallbackScriptFunction(script, function);
				if (script2)
					App->scripting->CallbackScriptFunction(script2, function);
			}
		}
	}
}

