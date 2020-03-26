#ifndef MODULE_PARTICLES_H__
#define MODULE_PARTICLES_H__

#include "Module.h"

BE_BEGIN_NAMESPACE
class ComponentParticleEmitter;
class BROKEN_API ModuleParticles : public Module
{
public:
	ModuleParticles(bool start_enabled);
	~ModuleParticles();

	bool Init(json& config) override;
	bool Start() override;
	update_status Update(float dt) override;

	void AddEmitter(ComponentParticleEmitter* componentEmitter);
	void DeleteEmitter(ComponentParticleEmitter* componentEmitter);

	bool CleanUp() override;

private:

	std::vector<ComponentParticleEmitter*> particleEmitters;
};

BE_END_NAMESPACE
#endif // _MODULE_PARTICLES_H__
