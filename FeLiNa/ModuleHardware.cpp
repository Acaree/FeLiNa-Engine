#include "ModuleHardware.h"

ModuleHardware::ModuleHardware(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}
ModuleHardware::~ModuleHardware()
{

}

void ModuleHardware::FillHardwareInfo()
{
	SDL_version sdl_version;
	SDL_VERSION(&sdl_version);

	sprintf_s(hardware_info.sdl_current_version, "%i.%i.%i", sdl_version.major, sdl_version.minor, sdl_version.patch);

	hardware_info.cpu_count = SDL_GetCPUCount();
	sprintf_s(hardware_info.cpu_version_count, sizeof(hardware_info.cpu_version_count), "%i", hardware_info.cpu_count);

	hardware_info.total_ram = SDL_GetSystemRAM() / 1024;
	sprintf_s(hardware_info.total_ram_count, sizeof(hardware_info.total_ram_count), "%i Gb", hardware_info.total_ram);

	hardware_info.RDTSC = SDL_HasRDTSC();
	hardware_info.MMX = SDL_HasMMX();
	hardware_info.AVX = SDL_HasAVX();
	hardware_info.AVX2 = SDL_HasAVX2();
	hardware_info.SSE = SDL_HasSSE();
	hardware_info.SSE2 = SDL_HasSSE2();
	hardware_info.SSE3 = SDL_HasSSE3();
	hardware_info.SSE41 = SDL_HasSSE41();
	hardware_info.SSE42 = SDL_HasSSE42();

	hardware_info.vendor = glGetString(GL_VENDOR);
	hardware_info.gpu_name = glGetString(GL_RENDERER);

	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &hardware_info.total_memory);
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &hardware_info.available_memory);
	glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &hardware_info.dedicated_memory);

	hardware_info.memory_usage = hardware_info.total_memory - hardware_info.available_memory;
}
