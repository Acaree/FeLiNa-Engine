#include "ModuleGui.h"
#include "Application.h"
#include "mmgr/mmgr.h"
#include "ModuleHardware.h"
//XD need module hardware-------------------------------------
#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX			0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX		0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX    0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX				0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX				0x904B
//-------------------------------------------------------------

ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleGui::~ModuleGui()
{
}

bool ModuleGui::Start()
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	ImGui::StyleColorsDark();

	return true;
}

update_status ModuleGui::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_return;
}

update_status ModuleGui::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui::ShowDemoWindow();

	ShowMainMenuBar();

	ShowConfigurationWindow();

	if (About_active) {
		ShowAboutWindow();
	}
	
	return update_return;
}

update_status ModuleGui::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	if (close_program)
		update_return = UPDATE_STOP;

	return update_return;
}

bool ModuleGui::CleanUp()
{

	return true;
}

void ModuleGui::ShowMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			//MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
			if (ImGui::MenuItem("Close FeLiNa", NULL, false, true))
			{
				close_program = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{


			ImGui::EndMenu();
		}


		if (ImGui::BeginMenu("Help"))
		{

			if (ImGui::MenuItem("Documentation", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Download", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine/releases", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("Report a bug", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine/issues", NULL, NULL, SW_SHOWNORMAL);
			}

			if (ImGui::MenuItem("About", NULL, false, true))
			{
				About_active = true;	//maybe guarrada
			}


			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}


void ModuleGui::ShowConfigurationWindow()
{
	ImGui::SetNextWindowSize({400,800});
	
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("Configuration", false, window_flags);

	if (ImGui::CollapsingHeader("Application"))
	{
		ImGui::InputText("App name",App->app_name,20);
		ImGui::InputText("Organization", App->organization, 20);
		if (vector_fps.size() != 100)
		{
			vector_fps.push_back(App->GetFPS());
			vector_ms.push_back(App->GetMS());
		}
		else
		{
			vector_fps.erase(vector_fps.begin());
			vector_fps.push_back(App->GetFPS());

			vector_ms.erase(vector_ms.begin());
			vector_ms.push_back(App->GetMS());
		}
		char title[30];

		sprintf_s(title, 25, "Framerate %.1f", vector_fps[vector_fps.size() - 1]);
		ImGui::PlotHistogram("##framerate", &vector_fps[0], vector_fps.size(),0,title,0.0f,100.0f,ImVec2(310,100));

		sprintf_s(title, 25, "Milliseconds %.1f", vector_ms[vector_ms.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &vector_ms[0], vector_ms.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

		sMStats stats = m_getMemoryStatistics();

		ImGui::Text("Total Reported Mem:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.totalReportedMemory);

		if (vector_memory.size() != 100)
		{
			vector_memory.push_back((int)stats.accumulatedActualMemory);
		}
		else
		{
			vector_memory.erase(vector_memory.begin());
			vector_memory.push_back((int)stats.accumulatedActualMemory);
		}

		sprintf_s(title, 25, "Memory Consumption ");
		ImGui::PlotHistogram("##Memory Consumption", &vector_memory[0], vector_memory.size(), 0, title, 0.0f, 1000000, ImVec2(310, 100));

		ImGui::Text("Total Actual Mem:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.accumulatedActualMemory);

		ImGui::Text("Peak Reported Mem:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.peakReportedMemory);

		ImGui::Text("Peak Actual Mem:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.peakActualMemory );

		ImGui::Text("Accumulated Reported Mem:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.accumulatedReportedMemory);

		ImGui::Text("Accumulated Actual Mem:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.accumulatedActualMemory);

		ImGui::Text("Accumulated Alloc Unit Count:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.accumulatedAllocUnitCount);

		ImGui::Text("Total Alloc Unit Count:");
		ImGui::SameLine();
		ImGui::Text("%i", stats.totalAllocUnitCount);

		ImGui::Text("Peak Alloc Unit Count");
		ImGui::SameLine();
		ImGui::Text("%i", stats.peakAllocUnitCount);
		
		
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		
		
		if (ImGui::SliderFloat("Brightness", &App->window->brightness,0.0f,1.0f))
		{
			int ll = SDL_SetWindowBrightness(App->window->window, App->window->brightness);
			SDL_UpdateWindowSurface(App->window->window);
			
		}

		if (ImGui::SliderInt("Width", &App->window->width, 1, 2000)  || ImGui::SliderInt("Height", &App->window->height, 1, 2000))
		{
			SDL_SetWindowSize(App->window->window, App->window->width, App->window->height);
			SDL_UpdateWindowSurface(App->window->window);
		}
		ImGui::Spacing();
		//¿Refresh rate?
		/*SDL_DisplayMode* display;

		int t = SDL_GetWindowDisplayIndex(App->window->window);
		int c = SDL_GetWindowDisplayMode(App->window->window, display);
		//int l = SDL_GetDisplayMode(0);
		SDL_REFRES
		ImGui::Text("Refresh Rate", t);*/
		//FULLSCREEN; REASIZABLE; BORDELESS; FULL DESKTOP
		if (ImGui::Checkbox("Fullscreen", &App->window->fullscreen))
		{
			if (App->window->fullscreen)
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN);
			else
				SDL_SetWindowFullscreen(App->window->window, App->window->flags);
		}
		ImGui::SameLine();
		//NEES SAVE/LOAD TO JSON
		if (ImGui::Checkbox("Resizable", &App->window->reasizable))
		{
		
		}

		if (ImGui::Checkbox("Bordeless", &App->window->bordeless))
		{
				SDL_SetWindowBordered(App->window->window,(SDL_bool)!App->window->bordeless);
		}
		ImGui::SameLine();
		if (ImGui::Checkbox("Fullscreen Desktop", &App->window->fullscreen_desktop))
		{
			if (App->window->fullscreen_desktop)
				SDL_SetWindowFullscreen(App->window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
			else
				SDL_SetWindowFullscreen(App->window->window, App->window->flags);
		}
	}
	if (ImGui::CollapsingHeader("Hardware Detection"))
	{
		App->hardware->FillHardwareInfo();
		ImGui::Text("SDL Version: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0,1,0,100), App->hardware->hardware_info.sdl_current_version);

		ImGui::Separator();

		ImGui::Text("CPUs: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), App->hardware->hardware_info.cpu_version_count);
		
		ImGui::Text("System RAM: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), App->hardware->hardware_info.total_ram_count);
		
		ImGui::Text("Caps: ");
		ImGui::SameLine();
		
		if (App->hardware->hardware_info.RDTSC)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "RDTSC");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.MMX)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "MMX");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.AVX)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "AVX");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.AVX2)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "AVX2");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.SSE)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.SSE2)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE2");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.SSE3)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE3");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.SSE41)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE41");
			ImGui::SameLine();
		}
		if (App->hardware->hardware_info.SSE42)
		{
			ImGui::TextColored(ImVec4(0, 1, 0, 100), "SSE42");

		}
		ImGui::Separator();
		//GPU
		ImGui::Text("GPU: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "%s", App->hardware->hardware_info.vendor);
		ImGui::Separator();
		//Brand
		ImGui::Text("Brand: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "%s", App->hardware->hardware_info.gpu_name);
		//VRAM Budget
		ImGui::Text("VRAM Budget: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (App->hardware->hardware_info.total_memory * 0.001));
		//VRAM Usage
		ImGui::Text("VRAM Usage: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (App->hardware->hardware_info.memory_usage * 0.001));
		//VRAM Available
		ImGui::Text("VRAM Available: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (App->hardware->hardware_info.available_memory * 0.001));
		//VRAM Reserved
		ImGui::Text("VRAM Reserved: ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1, 0, 100), "%.2f Mb", (App->hardware->hardware_info.dedicated_memory * 0.001));
		//----------------------------------- SEE AND CHANGE
	}

	ImGui::End();
}


void ModuleGui::ShowAboutWindow() {

	ImGui::SetNextWindowSize({ 400,100 });
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("About",&About_active, window_flags);

	ImGui::Text("This engine is made by two students in CITM.");
	ImGui::Text("This software is licensed under MIT license.");

	ImGui::End();

}