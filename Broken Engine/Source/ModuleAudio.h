#ifndef __ModuleAudio__H__
#define __ModuleAudio__H__

#include "Module.h"
#include "Math.h"
#include "Wwise/AK/SoundEngine/Common/AkTypes.h"
#include <vector>

BE_BEGIN_NAMESPACE
class BROKEN_API WwiseGameObject
{
public:
	WwiseGameObject(uint64 id, const char* name);
	~WwiseGameObject();

public:
	void SetPosition(float posX = 0, float posY = 0, float posZ = 0, float frontX = 1, float frontY = 0, float frontZ = 0, float topX = 0, float topY = 1, float topZ = 0);
	void PlayEvent(uint id);
	void PauseEvent(uint id);
	void ResumeEvent(uint id);
	void StopEvent(uint id);
	void SetVolume(uint id, float volume);
	WwiseGameObject* CreateAudioSource(uint id, const char* name, float3 position);
	WwiseGameObject* CreateAudioListener(uint id, const char* name, float3 position);
	void SetAuxSends();

public:
	uint GetID();

public:
	float volume = 1.0f;
	AkGameObjectID id = 0;
private:
	const char* name = nullptr;

	AkVector position = { 0,0,0 };
	AkVector orientationFront = { 0,0,0 };
	AkVector orientationTop = { 0,0,0 };
};

class BROKEN_API ModuleAudio : public Module
{
public:
	ModuleAudio(bool start_enabled = true);
	virtual ~ModuleAudio();

	bool Init(json& file);
	bool Start();
	update_status PostUpdate(float dt);
	bool CleanUp();
	void Tests(AkGameObjectID id);

private:

	void InitWwise();
	void TerminateWwise();
	void LoadSoundBank(const char* path);
	void LoadEventsFromJson();
	void StopAllAudioEvents();
	void ResumeAllAudioEvents();
	void PauseAllAudioEvents();

public:
	AkGameObjectID currentListenerID = 0;
	//unsigned long g_envMAP[255];
	std::map <std::string, uint> EventMap;
	std::vector<WwiseGameObject*> audioListenerList;
	std::vector<WwiseGameObject*> audioSourceList;
};
BE_END_NAMESPACE
#endif __ModuleAudio__H__