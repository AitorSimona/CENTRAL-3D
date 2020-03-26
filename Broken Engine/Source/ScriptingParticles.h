#ifndef __SCRIPTINGPARTICLES_H__
#define __SCRIPTINGPARTICLES_H__

#include "BrokenCore.h"

class lua_State;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingParticles {
public:
	ScriptingParticles();
	~ScriptingParticles();

public:
	void ActivateParticlesEmission() const;
	void DeactivateParticlesEmission() const;

};
BE_END_NAMESPACE
#endif // __SCRIPTINGPARTICLES_H__