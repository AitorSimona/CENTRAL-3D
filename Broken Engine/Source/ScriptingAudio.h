#ifndef __SCRIPTINGAUDIO_H__
#define __SCRIPTINGAUDIO_H__

#include "BrokenCore.h"

class lua_State;

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingAudio {
public:
	ScriptingAudio();
	~ScriptingAudio();

public:
	void PlayAttackSound();
	void PlayStepSound();
	void StopAttackSound();
	void StopStepSound();
	void SetVolume(float volume);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGAUDIO_H__