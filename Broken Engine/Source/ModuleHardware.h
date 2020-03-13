#ifndef __MODULEHARDWARE_H__
#define __MODULEHARDWARE_H__

#include "Module.h"
#include <string>
#define NOMINMAX
#include <windows.h>
#include "psapi.h"


#define BTOGB (1073741824.0f)
#define KBTOMB 1024.0f //To GB: (1048576.0f)
#define BTOMB (1048576.0f)

BE_BEGIN_NAMESPACE
//-------------------------- SOFTWARE INFO --------------------------//
class BROKEN_API SoftwareInfo {
private:

	//Don't use this, they are just class variables, use the getters instead
	mutable std::string mSoftware_CppVersion;
	mutable std::string mSoftware_LANGCppVersion;
	mutable std::string mSoftware_WindowsVersion;
	mutable std::string mSoftware_SDLVersion;
	mutable std::string mCppCompilerVersion;
	mutable std::string mMultithreadedSpecified;
	mutable std::string mCppNumericalVersion;
	mutable std::string mVsCompilerVersion;

private:

	//Methods to check the versions for different software -- DON'T USE THEM!
	const std::string ExtractCppVersion(long int cppValue);
	const std::string ExtractWindowsVersion();
	const std::string ExtractSDLVersion();

public:

	void DetectSystemProperties(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!

	//Methods to return the different values for software versions... Ready to print -- Use them :)
	const char* GetWindowsVersion()	const { return mSoftware_WindowsVersion.c_str(); }
	const char* OsFoundString()		const { return (__STDC_HOSTED__ ? "OS Found" : "OS NOT FOUND!"); }

	const char* GetSDLVersion()		const { return mSoftware_SDLVersion.c_str(); }
	const char* GetOGLVersion()				const;
	const char* GetOGLShadingVersion()		const;

	///__cplusplus returning values:
	///199711L (C++98 or C++03)
	///201103L (C++11)
	///201402L (C++14)
	///201703L (C++17)
	const char* GetCppVersionImplementedByCompiler()	const { return mSoftware_CppVersion.c_str(); }
	const char* GetCPPNumericalVersion()				const;
	const char* GetCppCompilerVersion()				const;


	const char* GetVSCompilerVersion()				const;
	const char* MultithreadedSpecified()				const;

	const char* GetCompilationDate()					const { return __DATE__; }
	const char* GetCompilationTime()					const { return __TIME__; }
};


//--------------------------- MEMORY INFO ---------------------------//
class BROKEN_API MemoryHardware {
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


//---------------------------- CPU INFO -----------------------------//
class BROKEN_API ProcessorHardware {
private:

	SYSTEM_INFO m_CpuSysInfo;
	std::string m_CPUBrand;
	std::string m_CPUVendor;

	std::string m_CpuArchitecture;
	std::string m_InstructionsSet;

	struct InstructionsSet {
		bool Available_3DNow = false;
		bool RDTSC_Available = false;
		bool AltiVec_Available = false;
		bool AVX_Available = false;
		bool AVX2_Available = false;
		bool MMX_Available = false;
		bool SSE_Available = false;
		bool SSE2_Available = false;
		bool SSE3_Available = false;
		bool SSE41_Available = false;
		bool SSE42_Available = false;
	};

	InstructionsSet m_CPUInstructionSet;

private:

	void GetCPUSystemInfo();
	const std::string ExtractCPUArchitecture(SYSTEM_INFO& SystemInfo);
	void CheckForCPUInstructionsSet();

public:

	void DetectSystemProperties(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!

	const uint GetCPUCores()						const { return SDL_GetCPUCount(); }
	const uint GetCPUCacheLine1Size()				const { return SDL_GetCPUCacheLineSize(); } //In bytes

	const char* GetCPUArchitecture()			const { return m_CpuArchitecture.c_str(); }
	const DWORD GetNumberOfProcessors()		const { return m_CpuSysInfo.dwNumberOfProcessors; }
	const WORD GetProcessorRevision()		const { return m_CpuSysInfo.wProcessorRevision; }

	const InstructionsSet& GetCPUInstructionsSet()	const { return m_CPUInstructionSet; }
	const char* GetCPUInstructionSet()		const;

	const char* GetCPUBrand()					const { return m_CPUBrand.c_str(); }
	const char* GetCPUVendor()				const { return m_CPUVendor.c_str(); }
};


//---------------------------- GPU INFO -----------------------------//
class BROKEN_API GPUHardware {
private:

	struct GPUPrimaryInfo_IntelGPUDetect {
		std::string m_GPUBrand;
		uint m_GPUID = 0;
		uint m_GPUVendor = 0;

		//'PI' is for "Primary Info", 'GPUDet' is for "GPUDetect", the code we use to get this info
		//Info in Bytes
		uint64 mPI_GPUDet_TotalVRAM_Bytes = 0;
		uint64 mPI_GPUDet_VRAMUsage_Bytes = 0;
		uint64 mPI_GPUDet_CurrentVRAM_Bytes = 0;
		uint64 mPI_GPUDet_VRAMReserved_Bytes = 0;

		//Info in MB (Just change the division of BTOMB in the GPUDetect_ExtractGPUInfo() function
		//by the BTOGB one to get the GB). And change this variables name!! (_MB change it by _GB!!!)
		float mPI_GPUDet_TotalVRAM_MB = 0.0f;
		float mPI_GPUDet_VRAMUsage_MB = 0.0f;
		float mPI_GPUDet_CurrentVRAM_MB = 0.0f;
		float mPI_GPUDet_VRAMReserved_MB = 0.0f;

	};

	mutable GPUPrimaryInfo_IntelGPUDetect m_PI_GPUDet_GPUInfo; //This is the Info for the current GPU in use

	//For OPENGL GPU "Getter"
	GLint m_GPUTotalVRAM = 0;
	GLint m_GPUCurrentVRAM = 0;

private:

	void GPUDetect_ExtractGPUInfo() const;

public:

	void DetectSystemProperties(); //DON'T USE THIS FUNCTION, IS JUST FOR CLASS PURPOSES!!!
	void RecalculateGPUParameters() const { GPUDetect_ExtractGPUInfo(); }

	const char* GetGPUBenchmark()	const { return (const char*)glGetString(GL_VENDOR); }
	const char* GetGPUModel()		const { return (const char*)glGetString(GL_RENDERER); }

	const GLint GetGPUTotalVRAM();  // In MB... Only for NVIDIA GPUs, otherwise returns 0
	const GLint GetGPUCurrentVRAM(); // In MB... Only for NVIDIA GPUs, otherwise returns 0

	const GPUPrimaryInfo_IntelGPUDetect& GetGPUInfo_GPUDet() const { return m_PI_GPUDet_GPUInfo; }
};



//-------------------------------------------------------------------//
struct BROKEN_API hw_info {
	SoftwareInfo Software_Information;
	MemoryHardware Memory_Information;
	ProcessorHardware CPU_Information;
	GPUHardware GPU_Information;

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

class BROKEN_API ModuleHardware : public Module {
public:

	ModuleHardware(bool start_enabled = true);
	~ModuleHardware();

	bool Start();

	const hw_info& GetInfo() const;
	const SoftwareInfo& GetSwInfo() const { return info.Software_Information; }
	const MemoryHardware& GetMemInfo() const { return info.Memory_Information; }
	const ProcessorHardware& GetProcessorInfo() const { return info.CPU_Information; }
	const GPUHardware& GetGraphicsCardInfo() const { return info.GPU_Information; }

	void RecalculateParameters() { info.Memory_Information.RecalculateRAMParameters(); info.GPU_Information.RecalculateGPUParameters(); }

private:

	mutable hw_info info;
};

BE_END_NAMESPACE
#endif // __MODULEHARDWARE_H__