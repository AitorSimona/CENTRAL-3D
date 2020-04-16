#ifndef __SCRIPTINGAUDIO_H__
#define __SCRIPTINGAUDIO_H__

#include "BrokenCore.h"

BE_BEGIN_NAMESPACE
class BROKEN_API ScriptingAudio {
public:
	ScriptingAudio();
	~ScriptingAudio();

public:
	void SetVolume(float volume);
	void PlayAudioEvent(std::string event);
	void StopAudioEvent(std::string event);
	void PauseAudioEvent(std::string event);
	void ResumeAudioEvent(std::string event);

};
BE_END_NAMESPACE
#endif // __SCRIPTINGAUDIO_H__