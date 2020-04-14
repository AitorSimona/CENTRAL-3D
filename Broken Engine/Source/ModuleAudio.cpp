#include "ModuleAudio.h"
#include <string>
#include "wwise.h"
#include "Wwise/IO/Win32/AkFilePackageLowLevelIOBlocking.h"
#include "..\Game\Assets\Sounds\Wwise_IDs.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "JSONLoader.h"
#include <vector>

#include "mmgr/mmgr.h"

#define BANKNAME_INIT "Assets/Sounds/Init.bnk"

using namespace Broken;

CAkDefaultIOHookBlocking g_lowLevelIO;

ModuleAudio::ModuleAudio(bool start_enabled) : Module(start_enabled) {
	name = "Audio";
}

ModuleAudio::~ModuleAudio() 
{
	for (int i = 0; i < audioListenerList.size(); ++i)
	{
		delete audioListenerList[i];
		audioListenerList[i] = nullptr;
	}
	for (int i = 0; i < audioSourceList.size(); ++i)
	{
		delete audioSourceList[i];
		audioSourceList[i] = nullptr;
	}
}

bool ModuleAudio::Init(json& file)
{
	InitWwise();

	LoadSoundBank("Main");

	return true;
}

bool ModuleAudio::Start()
{
	LoadEventsFromJson();

	return true;
}

update_status ModuleAudio::PostUpdate(float dt)
{
	AK::SoundEngine::RenderAudio();
	if (App->GetAppState() == Broken::AppState::TO_PAUSE)
	{
		PauseAllAudioEvents();
	}
	if (App->GetAppState() == Broken::AppState::TO_EDITOR)
	{
		StopAllAudioEvents();
	}
	if (App->GetAppState() == Broken::AppState::TO_PLAY)
	{
		ResumeAllAudioEvents();
	}
	return UPDATE_CONTINUE;
}

bool ModuleAudio::CleanUp()
{
	TerminateWwise();
	return true;
}

void ModuleAudio::InitWwise()
{
	// Initialize audio engine
	// Memory
	AkMemSettings memSettings;

	memSettings.uMaxNumPools = 20;

	// Streaming.
	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Device Settings
	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);


	

	// Sound Engine
	AkInitSettings l_InitSettings;
	AkPlatformInitSettings l_platInitSetings;
	AK::SoundEngine::GetDefaultInitSettings(l_InitSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(l_platInitSetings);

	// Setting pool sizes for this game. Here, allow for user content; every game should determine its own optimal values.
	l_InitSettings.uDefaultPoolSize = 2 * 1024 * 1024;
	l_platInitSetings.uLEngineDefaultPoolSize = 4 * 1024 * 1024;
	
	// Music Engine
	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	// Create and initialise an instance of our memory manager.
	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		AKASSERT(!"Could not create the memory manager.");
		return;
	}

	// Create and initialise an instance of the default stream manager.
	if (!AK::StreamMgr::Create(stmSettings))
	{
		AKASSERT(!"Could not create the Stream Manager");
		return;
	}

	// Create an IO device.
	if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		AKASSERT(!"Cannot create streaming I/O device");
		return;
	}

	// Initialize sound engine.
	if (AK::SoundEngine::Init(&l_InitSettings, &l_platInitSetings) != AK_Success)
	{
		AKASSERT(!"Cannot initialize sound engine");
		return;
	}

	// Initialize music engine.
	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		AKASSERT(!"Cannot initialize music engine");
		return;
	}

#ifndef AK_OPTIMIZED
	// Initialize communication.
	AkCommSettings settingsComm;
	AK::Comm::GetDefaultInitSettings(settingsComm);
	if (AK::Comm::Init(settingsComm) != AK_Success)
	{
		AKASSERT(!"Cannot initialize music communication");
		return;
}
#endif // AK_OPTIMIZED

	AkBankID bankID;
	AKRESULT retValue;
	retValue = AK::SoundEngine::LoadBank(BANKNAME_INIT, AK_DEFAULT_POOL_ID, bankID);
	//assert(retValue == AK_Success);
}

void ModuleAudio::TerminateWwise()
{
	// Terminate audio engine

#ifndef AK_OPTIMIZED
	// Terminate Communication Services
	AK::Comm::Term();
#endif // AK_OPTIMIZED

	// Terminate Music Engine
	AK::MusicEngine::Term();

	// Terminate Sound Engine
	AK::SoundEngine::Term();

	// Terminate Streaming Manager
	g_lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
		AK::IAkStreamMgr::Get()->Destroy();

	// Terminate Memory Manager
	AK::MemoryMgr::Term();
}

void ModuleAudio::LoadSoundBank(const char* path)
{
	std::string fullPath = "Assets/Sounds/";
	fullPath += path;
	fullPath += ".bnk";

	AkBankID bankID;

	if(App->fs->Exists(fullPath.c_str()))
	AK::SoundEngine::LoadBank(fullPath.c_str(), AK_DEFAULT_POOL_ID, bankID);
}

void ModuleAudio::Tests(AkGameObjectID id)
{
	//SoundEngine::SetPanningRule(AkPanningRule_Speakers);
	//SoundEngine::SetVolumeThreshold();
	//SoundEngine::SetOutputVolume();
	//SoundEngine::SetRTPCValue();

	AkAuxSendValue reverb;
	reverb.listenerID = AK_INVALID_GAME_OBJECT;
	reverb.auxBusID = AK::AUX_BUSSES::REVERB;
	reverb.fControlValue = 1.0f;

	AK::SoundEngine::SetGameObjectAuxSendValues(id, NULL, 0);
}

WwiseGameObject::WwiseGameObject(uint64 id, const char* name)
{
	this->id = id;
	this->name = name;

	AK::SoundEngine::RegisterGameObj(this->id, this->name);
}

WwiseGameObject::~WwiseGameObject()
{
	//AK::SoundEngine::UnregisterGameObj(id);
}

void WwiseGameObject::SetPosition(float posX, float posY, float posZ, float frontX, float frontY, float frontZ, float topX, float topY, float topZ)
{
	position.X = -posX;
	position.Y = posY;
	position.Z = -posZ;

	orientationFront.X = frontX;
	orientationFront.Y = frontY;
	orientationFront.Z = frontZ;

	orientationTop.X = topX;
	orientationTop.Y = topY;
	orientationTop.Z = topZ;

	AkSoundPosition soundPosition;
	soundPosition.Set(position, orientationFront, orientationTop);
	AK::SoundEngine::SetPosition(id, soundPosition);
}

void WwiseGameObject::PlayEvent(uint id)
{
	AK::SoundEngine::PostEvent(id, this->id);
}

void WwiseGameObject::PauseEvent(uint id)
{
	AK::SoundEngine::ExecuteActionOnEvent(id, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Pause, this->id);
}

void WwiseGameObject::ResumeEvent(uint id)
{
	AK::SoundEngine::ExecuteActionOnEvent(id, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Resume, this->id);
}

void WwiseGameObject::StopEvent(uint id)
{
	AK::SoundEngine::ExecuteActionOnEvent(id, AK::SoundEngine::AkActionOnEventType::AkActionOnEventType_Stop, this->id);
}

void WwiseGameObject::SetVolume(uint id, float volume)
{
	AK::SoundEngine::SetGameObjectOutputBusVolume(this->id, AK_INVALID_GAME_OBJECT, volume);
	this->volume = volume;
}

WwiseGameObject* WwiseGameObject::CreateAudioSource(uint id, const char* name, float3 position)
{
	WwiseGameObject* go = new WwiseGameObject(id, name);
	go->SetPosition(position.x, position.y, position.z);
	App->audio->audioSourceList.push_back(go);

	return go;
}

WwiseGameObject* WwiseGameObject::CreateAudioListener(uint id, const char* name, float3 position)
{
	WwiseGameObject* go = new WwiseGameObject(id, name);

	AkGameObjectID listenerID = go->GetID();
	AK::SoundEngine::SetDefaultListeners(&listenerID, 1);
	go->SetPosition(position.x, position.y, position.z);
	App->audio->currentListenerID = listenerID;
	App->audio->audioListenerList.push_back(go);

	return go;
}

void WwiseGameObject::SetAuxSends()
{
	AkAuxSendValue reverb[1];
	reverb[0].listenerID = AK_INVALID_GAME_OBJECT;
	reverb[0].auxBusID = AK::AUX_BUSSES::REVERB;
	reverb[0].fControlValue = 1.0f;

	AKRESULT ret;
	ret = AK::SoundEngine::SetGameObjectAuxSendValues(id, reverb, 1);
	assert(ret == AK_Success);
}

uint WwiseGameObject::GetID()
{
	return id;
}

void ModuleAudio::LoadEventsFromJson()
{
	uint Id = 0;
	std::string name;
	json File = App->GetJLoader()->Load("Assets/Sounds/Main.json");
	
	json Events = File["SoundBanksInfo"]["SoundBanks"][0]["IncludedEvents"];
	EventMap.begin();

	for (uint i = 0; i < Events.size(); ++i)
	{
		json node = Events[i];

		std::string idstring = node["Id"];
		std::string namestring = node["Name"];
		Id = std::stoul(idstring);

		EventMap.insert(std::pair<std::string, uint>(namestring, Id));
	}

	uint i = 0;
}

void ModuleAudio::StopAllAudioEvents()
{
	AK::SoundEngine::StopAll();
}

void  ModuleAudio::ResumeAllAudioEvents()
{
	AK::SoundEngine::WakeupFromSuspend();
}

void  ModuleAudio::PauseAllAudioEvents()
{
	AK::SoundEngine::Suspend();
}