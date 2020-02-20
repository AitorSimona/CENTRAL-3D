#include "Globals.h"
#include "ModuleHardware.h"
#include "SDL/include/SDL.h"

#include "OpenGL.h"

#include "mmgr/mmgr.h"

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

