#ifndef _WWISE_H_
#define _WWISE_H_

#include "Wwise/AK/SoundEngine/Common/AkSoundEngine.h"
#include "Wwise/AK/MusicEngine/Common/AkMusicEngine.h"
#include "Wwise/AK/SoundEngine/Common/AkMemoryMgr.h"
#include "Wwise/AK/SoundEngine/Common/AkModule.h"
#include "Wwise/AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "Wwise/AK/SoundEngine/Common/IAkStreamMgr.h"
#include "Wwise/AK/Tools/Common/AkPlatformFuncs.h"

// Input libraries
#ifndef _DEBUG  
#define AK_OPTIMIZED
#pragma comment( lib, "Wwise/Release(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment( lib, "Wwise/Release(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment( lib, "Wwise/Release(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment( lib, "Wwise/Release(StaticCRT)/lib/AkStreamMgr.lib")
#else
#include "Wwise/AK/Comm/AkCommunication.h"
#pragma comment( lib, "Wwise/Debug(StaticCRT)/lib/CommunicationCentral.lib")
#pragma comment( lib, "Wwise/ws2_32.lib")
#pragma comment( lib, "Wwise/Debug(StaticCRT)/lib/AkSoundEngine.lib")
#pragma comment( lib, "Wwise/Debug(StaticCRT)/lib/AkMusicEngine.lib")
#pragma comment( lib, "Wwise/Debug(StaticCRT)/lib/AkMemoryMgr.lib")
#pragma comment( lib, "Wwise/Debug(StaticCRT)/lib/AkStreamMgr.lib")
#endif

// Microsoft DirectX
#pragma comment( lib, "Wwise/dxguid.lib")


// Custom alloc/free functions. These are declared as "extern" in AkMemoryMgr.h
// and MUST be defined by the game developer.
namespace AK
{
#ifdef WIN32
	void * AllocHook(size_t in_size);
	void FreeHook(void * in_ptr);
	void * VirtualAllocHook(void * in_pMemAddress, size_t in_size, DWORD in_dwAllocationType, DWORD in_dwProtect);
	void VirtualFreeHook(void * in_pMemAddress, size_t in_size, DWORD in_dwFreeType);
#endif
}

#endif _WWISE_H_
