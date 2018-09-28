#ifndef _MODULE_HARDWARE_
#define _MODULE_HARDWARE_

#include "Module.h"

#include "imgui-1.65/imgui.h"

#include "SDL/include/SDL_version.h"
#include "SDL/include/SDL_opengl.h"
#include "SDL/include/SDL_cpuinfo.h"

#include "mmgr/mmgr.h"

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX			0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX		0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX    0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX				0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX				0x904B

class ModuleHardware : public Module
{
public:
	ModuleHardware(Application* parent, bool start_enabled = true);
	~ModuleHardware();
	
	void DrawHardwareInformationPanel();
private:

	char sdl_current_version[10];
	//CPU
	int cpu_count = 0;
	char cpu_version_count[10];
	int total_ram = 0;
	char total_ram_count[10];
	bool RDTSC = false;
	bool MMX = false;
	bool AVX = false;
	bool AVX2 = false;
	bool SSE = false;
	bool SSE2 = false;
	bool SSE3 = false;
	bool SSE41 = false;
	bool SSE42 = false;
	//GPU
	const GLubyte *vendor;
	const GLubyte *gpu_name;
	GLint total_memory = 0;
	GLint memory_usage = 0;
	GLint dedicated_memory = 0;
	GLint available_memory = 0;
	
	void FillHardwareInfo();
	bool fill_hardware = false;
};



#endif

