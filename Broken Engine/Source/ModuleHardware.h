#ifndef __MODULEHARDWARE_H__
#define __MODULEHARDWARE_H__

#include "Module.h"
#include <string>

struct hw_info {
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

private:

	mutable hw_info info;
};

#endif // __MODULEHARDWARE_H__