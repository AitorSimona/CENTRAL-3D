#include "Globals.h"
#include "ModuleHardware.h"
#include "SDL/include/SDL.h"

#include "OpenGL.h"

#include <codecvt> //To convert wstring to string (For GPU info)

#include "mmgr/mmgr.h"

static sMStats m_MemoryInfo_StatsFromMMRG;
void RecalculateMemStatisticsFromMMGR()
{
	m_MemoryInfo_StatsFromMMRG = m_getMemoryStatistics();
}

ModuleHardware::ModuleHardware(bool start_enabled) : Module(start_enabled)
{
	// --- Retrieve SDL Version ---
	SDL_version version;
	SDL_GetVersion(&version);
	sprintf_s(info.sdl_version, 25, "%i.%i.%i", version.major, version.minor, version.patch);
	
	// --- Retrieve CPU and Memory Information ---
	info.ram_gb = (float)SDL_GetSystemRAM() / (1024.f);
	info.cpu_count = SDL_GetCPUCount();
	info.l1_cachekb = SDL_GetCPUCacheLineSize();
	info.rdtsc = SDL_HasRDTSC() == SDL_TRUE;
	info.altivec = SDL_HasAltiVec() == SDL_TRUE;
	info.now3d = SDL_Has3DNow() == SDL_TRUE;
	info.mmx = SDL_HasMMX() == SDL_TRUE;
	info.sse = SDL_HasSSE() == SDL_TRUE;
	info.sse2 = SDL_HasSSE2() == SDL_TRUE;
	info.sse3 = SDL_HasSSE3() == SDL_TRUE;
	info.sse41 = SDL_HasSSE41() == SDL_TRUE;
	info.sse42 = SDL_HasSSE42() == SDL_TRUE;
	info.avx = SDL_HasAVX() == SDL_TRUE;
	info.avx2 = SDL_HasAVX2() == SDL_TRUE;

	info.Memory_Information.DetectSystemProperties();
	info.Software_Information.DetectSystemProperties();
}

// Destructor
ModuleHardware::~ModuleHardware()
{}

const hw_info & ModuleHardware::GetInfo() const
{
	// --- Retrieve GPU Information ---
	const GLubyte* GPUvendor = glGetString(GL_VENDOR);
	const GLubyte* GPU = glGetString(GL_RENDERER);
	const GLubyte* GPUdriver = glGetString(GL_VERSION);

	// --- NVIDIA EXTENSION!!! --- 
	GLint video_mem_budget = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &video_mem_budget);

	GLint video_mem_available = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &video_mem_available);

	GLint video_mem_usage = 0;
	video_mem_usage = video_mem_budget - video_mem_available;

	info.gpu_vendor.assign((const char*)GPUvendor);
	info.gpu_brand.assign((const char*)GPU);
	info.gpu_driver.assign((const char*)GPUdriver);

	info.vram_mb_budget = float(video_mem_budget) / (1024.0f);
	info.vram_mb_usage = float(video_mem_usage) / (1024.f);
	info.vram_mb_available = float(video_mem_available) / (1024.f);

	return info;
}

//-------------------------------------------------------------------//
//-------------------------- SOFTWARE INFO --------------------------//
//-------------------------------------------------------------------//
void SoftwareInfo::DetectSystemProperties()
{
	mSoftware_CppVersion = ExtractCppVersion(__cplusplus);
	mSoftware_LANGCppVersion = ExtractCppVersion(_MSVC_LANG);
	mSoftware_WindowsVersion = ExtractWindowsVersion();
	mSoftware_SDLVersion = ExtractSDLVersion();
}


const std::string SoftwareInfo::ExtractCppVersion(long int cppValue)
{
	std::string tmp_cppVersion = "NULL: return value does not match with any C++ version!";
	switch (cppValue) {

	case(199711L):
		tmp_cppVersion = "C++ 98 or C++03";
		break;
	case(201103L):
		tmp_cppVersion = "C++11";
		break;
	case(201402L):
		tmp_cppVersion = "C++14";
		break;
	case(201703L):
		tmp_cppVersion = "C++17";
		break;
	default:
		tmp_cppVersion = "NULL: return value does not match with any C++ version!";
		break;
	}

	return tmp_cppVersion;
}


const std::string SoftwareInfo::ExtractWindowsVersion()
{
	OSVERSIONINFOEX OS;
	ZeroMemory(&OS, sizeof(OSVERSIONINFOEX));
	OS.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx(&(OSVERSIONINFO&)OS);

	std::string ret = "Windows ";

	if (OS.dwMajorVersion == 10)
		ret += "10";
	else if (OS.dwMajorVersion == 6) {

		if (OS.dwMinorVersion == 3)
			ret += "8.1";
		else if (OS.dwMinorVersion == 2)
			ret += "8";
		else if (OS.dwMinorVersion == 1)
			ret += "7";
		else
			ret += "Vista";
	}
	else if (OS.dwMajorVersion == 5) {

		if (OS.dwMinorVersion == 2)
			ret += "XP SP2";
		else if (OS.dwMinorVersion == 1)
			ret += "XP";
		else if (OS.dwMinorVersion == 0)
			ret += "2000";
	}
	else if (OS.dwMajorVersion == 4 || OS.dwMajorVersion == 3)
		ret += "WinNT";
	else
		ret = "WINDOWS VERSION NOT FOUND";

	return ret;
}


const std::string SoftwareInfo::ExtractSDLVersion()
{
	SDL_version linked;
	SDL_version compiled;

	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	std::string VersionString = "SDL Compiled Version " + std::to_string(compiled.major) + '.'
		+ std::to_string(compiled.minor) + '.' + std::to_string(compiled.patch);

	VersionString += ("\nSDL Linked Version " + std::to_string(linked.major) + '.'
		+ std::to_string(linked.minor) + '.' + std::to_string(linked.patch));

	return VersionString;
}


const std::string SoftwareInfo::GetCppCompilerVersion() const
{
	return (mSoftware_LANGCppVersion + " (" + std::to_string(_MSVC_LANG) + ")");
}


//-------------------------------------------------------------------//
//--------------------------- MEMORY INFO ---------------------------//
//-------------------------------------------------------------------//
void MemoryHardware::DetectSystemProperties()
{
	ExtractMemoryInfo();
	RecalculateMemStatisticsFromMMGR();
}


void MemoryHardware::ExtractMemoryInfo() const
{
	MEMORYSTATUSEX tmpMemoryInfo;
	tmpMemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&tmpMemoryInfo);

	m_MemoryInfo = tmpMemoryInfo;
	m_MemoryInfo.dwLength = sizeof(MEMORYSTATUSEX);

	GetProcessMemoryInfo(GetCurrentProcess(), &m_ProcessMemCounters, sizeof(m_ProcessMemCounters));
	mProcess_vMemUsed = m_ProcessMemCounters.PagefileUsage;
	mProcess_physMemUsed = m_ProcessMemCounters.WorkingSetSize;
}


void MemoryHardware::RecalculateRAMParameters()
{
	DetectSystemProperties();
	RecalculateMemStatisticsFromMMGR();
}

//Getting Stats of Memory from MMRG
const uint MemoryHardware::GetMemStatsFromMMGR_TotalReportedMemory()		const { return m_MemoryInfo_StatsFromMMRG.totalReportedMemory; }
const uint MemoryHardware::GetMemStatsFromMMGR_TotalActualMemory()			const { return m_MemoryInfo_StatsFromMMRG.totalActualMemory; }
const uint MemoryHardware::GetMemStatsFromMMGR_PeakReportedMemory()			const { return m_MemoryInfo_StatsFromMMRG.peakReportedMemory; }
const uint MemoryHardware::GetMemStatsFromMMGR_PeakActualMemory()			const { return m_MemoryInfo_StatsFromMMRG.peakActualMemory; }

const uint MemoryHardware::GetMemStatsFromMMGR_AccumulatedReportedMemory()	const { return m_MemoryInfo_StatsFromMMRG.accumulatedReportedMemory; }
const uint MemoryHardware::GetMemStatsFromMMGR_AccumulatedActualMemory()	const { return m_MemoryInfo_StatsFromMMRG.accumulatedActualMemory; }

const uint MemoryHardware::GetMemStatsFromMMGR_AccumulatedAllocUnitCount()	const { return m_MemoryInfo_StatsFromMMRG.accumulatedAllocUnitCount; }
const uint MemoryHardware::GetMemStatsFromMMGR_TotalAllocUnitCount()		const { return m_MemoryInfo_StatsFromMMRG.totalAllocUnitCount; }
const uint MemoryHardware::GetMemStatsFromMMGR_PeakAllocUnitCount()			const { return m_MemoryInfo_StatsFromMMRG.peakAllocUnitCount; }
