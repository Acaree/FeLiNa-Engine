#include "ModuleHardware.h"
#include "SDL/include/SDL_cpuinfo.h"
#include "SDL/include/SDL_version.h"
#include "ImGui/imgui.h"
#include "mmgr/mmgr.h"

#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX			0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX		0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX    0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX				0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX				0x904B

ModuleHardware::ModuleHardware(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Hardware";
}
ModuleHardware::~ModuleHardware()
{

}

void ModuleHardware::FillHardwareInfo()
{
	
	SDL_version sdl_version;
	SDL_VERSION(&sdl_version);

	sprintf_s(sdl_current_version, "%i.%i.%i", sdl_version.major, sdl_version.minor, sdl_version.patch);

	cpu_count = SDL_GetCPUCount();
	sprintf_s(cpu_version_count, sizeof(cpu_version_count), "%i", cpu_count);

	total_ram = SDL_GetSystemRAM() / 1024;
	sprintf_s(total_ram_count, sizeof(total_ram_count), "%i Gb", total_ram);

	RDTSC = SDL_HasRDTSC();
	MMX = SDL_HasMMX();
	AVX = SDL_HasAVX();
	AVX2 = SDL_HasAVX2();
	SSE = SDL_HasSSE();
	SSE2 = SDL_HasSSE2();
	SSE3 = SDL_HasSSE3();
	SSE41 = SDL_HasSSE41();
	SSE42 = SDL_HasSSE42();

	vendor = glGetString(GL_VENDOR);
	gpu_name = glGetString(GL_RENDERER);

	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_memory);
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &available_memory);
	glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicated_memory);

	memory_usage = total_memory - available_memory;

	fill_hardware = true;
}

void ModuleHardware::DrawHardwareInformationPanel() 
{
	if (!fill_hardware)
		FillHardwareInfo();
	
	ImGui::Text("SDL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100),sdl_current_version);

	ImGui::Separator();

	ImGui::Text("CPUs: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), cpu_version_count);

	ImGui::Text("System RAM: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), total_ram_count);

	ImGui::Text("Caps: ");
	ImGui::SameLine();

	if (RDTSC)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "RDTSC");
		ImGui::SameLine();
	}
	if (MMX)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "MMX");
		ImGui::SameLine();
	}
	if (AVX)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "AVX");
		ImGui::SameLine();
	}
	if (AVX2)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "AVX2");
		ImGui::SameLine();
	}
	if (SSE)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE");
		ImGui::SameLine();
	}
	if (SSE2)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE2");
		ImGui::SameLine();
	}
	if (SSE3)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE3");
		ImGui::SameLine();
	}
	if (SSE41)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE41");
		ImGui::SameLine();
	}
	if (SSE42)
	{
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE42");

	}
	ImGui::Separator();
	//GPU
	ImGui::Text("GPU: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), "%s", vendor);
	ImGui::Separator();
	//Brand
	ImGui::Text("Brand: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), "%s", gpu_name);
	//VRAM Budget
	ImGui::Text("VRAM Budget: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (total_memory * 0.001));
	//VRAM Usage
	ImGui::Text("VRAM Usage: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (memory_usage * 0.001));
	//VRAM Available
	ImGui::Text("VRAM Available: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (available_memory * 0.001));
	//VRAM Reserved
	ImGui::Text("VRAM Reserved: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (dedicated_memory * 0.001));
	
}

