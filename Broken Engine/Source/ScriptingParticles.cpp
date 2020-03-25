#include "ScriptingParticles.h"
#include "Application.h"
#include "ModuleScripting.h"
#include "ComponentParticleEmitter.h"
#include "ComponentDynamicRigidBody.h"
#include "ScriptData.h"

using namespace Broken;
ScriptingParticles::ScriptingParticles() {}

ScriptingParticles::~ScriptingParticles() {}

// PARTICLES ----------------------------------------------------------
void ScriptingParticles::ActivateParticlesEmission() const
{
	ComponentParticleEmitter* emmiter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emmiter && !emmiter->IsEnabled())
	{
		emmiter->Enable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Enabled");
	}
	else if (emmiter && emmiter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Enabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}

void ScriptingParticles::DeactivateParticlesEmission() const
{
	ComponentParticleEmitter* emmiter = App->scripting->current_script->my_component->GetContainerGameObject()->GetComponent<ComponentParticleEmitter>();

	if (emmiter && emmiter->IsEnabled())
	{
		emmiter->Disable();
		ENGINE_CONSOLE_LOG("[Script]: Particles Emission Disabled");
	}
	else if (emmiter && !emmiter->IsEnabled())
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component already Disabled");
	else
		ENGINE_CONSOLE_LOG("[Script]: Particle Emmiter component is NULL");
}