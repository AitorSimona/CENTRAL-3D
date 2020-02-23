#ifndef __MODULEHARDWARE_H__
#define __MODULEHARDWARE_H__

#include "Module.h"
#include <string>
#include "psapi.h"

#define BTOGB (1073741824.0f)
#define KBTOMB 1024.0f //To GB: (1048576.0f)
#define BTOMB (1048576.0f)

//-------------------------- SOFTWARE INFO --------------------------//
class SoftwareInfo
{
private:

	//Don't use this, they are just class variables, use the getters instead
	mutable std::string mSoftware_CppVersion;
	mutable std::string mSoftware_LANGCppVersion;
	mutable std::string mSoftware_WindowsVersion;
	mutable std::string mSoftware_SDLVersion;

private:

	//Methods to check the versions for different software -- DON'T USE THEM!
	const std::string ExtractCppVersion(long int cppValue);
	const std::string ExtractWindowsVersion();
	const std::string ExtractSDLVersion();

public:

	void DetectSystemProperties(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!

	//Methods to return the different values for software versions... Ready to print -- Use them :)
	const std::string GetWindowsVersion()	const { return mSoftware_WindowsVersion; }
	const std::string OsFoundString()		const { return (__STDC_HOSTED__ ? "OS Found" : "OS NOT FOUND!"); }

	const std::string GetSDLVersion()		const { return mSoftware_SDLVersion; }
	const auto GetOGLVersion()				const { return glGetString(GL_VERSION); }
	const auto GetOGLShadingVersion()		const { return glGetString(GL_SHADING_LANGUAGE_VERSION); }

	///__cplusplus returning values:
	///199711L (C++98 or C++03)
	///201103L (C++11)
	///201402L (C++14)
	///201703L (C++17)
	const std::string GetCppVersionImplementedByCompiler()	const { return mSoftware_CppVersion; }
	const std::string GetCPPNumericalVersion()				const { return std::to_string(__cplusplus); }
	const std::string GetCppCompilerVersion()				const;


	const std::string GetVSCompilerVersion()				const { return (std::to_string(_MSC_VER)); }
	const std::string MultithreadedSpecified()				const { return ("Multithreaded Specified: " + std::string(_MT ? "YES" : "NO")); }

	const std::string GetCompilationDate()					const { return __DATE__; }
	const std::string GetCompilationTime()					const { return __TIME__; }
};



//--------------------------- MEMORY INFO ---------------------------//
class MemoryHardware
{
private:

	mutable MEMORYSTATUSEX m_MemoryInfo;

	mutable PROCESS_MEMORY_COUNTERS m_ProcessMemCounters;
	mutable SIZE_T mProcess_vMemUsed;
	mutable SIZE_T mProcess_physMemUsed;

	void ExtractMemoryInfo() const;

public:

	void DetectSystemProperties(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!
	void RecalculateRAMParameters();
	//void RecalculateRAMParameters()										{ ExtractMemoryInfo(); m_MemoryInfo_StatsFromMMRG = m_getMemoryStatistics(); }

	const float GetRAMSizeFromSDL()								const { return (float)SDL_GetSystemRAM() / KBTOMB; } //In GB

	//Getting Stats of Memory from MEMORYSTATUSEX
	const uint32 GetRAMSize()									const { return (uint32)(m_MemoryInfo.dwLength); } //In GB
	const uint32 GetPercentageOfMemoryLoad()					const { return (uint32)(m_MemoryInfo.dwMemoryLoad); } //In %

	const uint64 GetPhysicalMemory()							const { return (uint64)(m_MemoryInfo.ullTotalPhys / BTOGB); } //In GB
	const uint64 GetFreePhysicalMemory()						const { return (uint64)(m_MemoryInfo.ullAvailPhys / BTOGB); } //In GB
	const uint64 GetUsedPhysicalMemory()						const { return (uint64)((m_MemoryInfo.ullTotalPhys - m_MemoryInfo.ullAvailPhys) / BTOGB); } //In GB

	const uint64 GetVirtualMemory()								const { return (uint64)(m_MemoryInfo.ullTotalVirtual / BTOGB); } //In GB
	const uint64 GetFreeVirtualMemory()							const { return (uint64)(m_MemoryInfo.ullAvailVirtual / BTOGB); } //In GB

	const uint64 GetFreeExtendedMemory()						const { return (uint64)(m_MemoryInfo.ullAvailExtendedVirtual / BTOMB); } //In MB

	const uint64 GetPageFileMemory()							const { return (uint64)(m_MemoryInfo.ullTotalPageFile / BTOGB); } //In GB
	const uint64 GetFreePageFileMemory()						const { return (uint64)(m_MemoryInfo.ullAvailPageFile / BTOGB); } //In GB

	const uint GetVirtualMemoryUsedByProcess()					const { return(uint)(mProcess_vMemUsed / BTOMB); } //In GB
	const uint GetPhysMemoryUsedByProcess()						const { return(uint)(mProcess_physMemUsed / BTOMB); } //In GB

	//Getting Stats of Memory from MMRG
	const uint GetMemStatsFromMMGR_TotalReportedMemory()		const;
	const uint GetMemStatsFromMMGR_TotalActualMemory()			const;
	const uint GetMemStatsFromMMGR_PeakReportedMemory()			const;
	const uint GetMemStatsFromMMGR_PeakActualMemory()			const;

	const uint GetMemStatsFromMMGR_AccumulatedReportedMemory()	const;
	const uint GetMemStatsFromMMGR_AccumulatedActualMemory()	const;

	const uint GetMemStatsFromMMGR_AccumulatedAllocUnitCount()	const;
	const uint GetMemStatsFromMMGR_TotalAllocUnitCount()		const;
	const uint GetMemStatsFromMMGR_PeakAllocUnitCount()			const;

};



//-------------------------------------------------------------------//
struct hw_info
{
	SoftwareInfo Software_Information;
	MemoryHardware Memory_Information;

	char sdl_version[25] = "";
	float ram_gb = 0.f;
	uint cpu_count = 0;
	uint l1_cachekb = 0;
	bool rdtsc = false;
	bool altivec = false;
	bool now3d = false;
	bool mmx = false;
	bool sse = false;
	bool sse2 = false;
	bool sse3 = false;
	bool sse41 = false;
	bool sse42 = false;
	bool avx = false;
	bool avx2 = false;
	std::string gpu_vendor;
	std::string gpu_driver;
	std::string gpu_brand;
	float vram_mb_budget = 0.f;
	float vram_mb_usage = 0.f;
	float vram_mb_available = 0.f;
	float vram_mb_reserved = 0.f;
};

class ModuleHardware : public Module
{

public:

	ModuleHardware(bool start_enabled = true);
	~ModuleHardware();

	const hw_info& GetInfo() const;
	const SoftwareInfo GetSwInfo() const { return info.Software_Information; }
	const MemoryHardware GetMemInfo() const { return info.Memory_Information; }

private:

	mutable hw_info info;
};

#endif // __MODULEHARDWARE_H__