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

	if(open_configuration)
		ShowConfigurationWindow();

	ShowLogWindow();

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
			
			if (ImGui::MenuItem("Close FeLiNa", NULL, false, true))
			{
				close_program = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Configuration", NULL, false, true))
			{
				open_configuration = !open_configuration;
			}
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
		App->DrawApplicationInformationPanel();
	}

	if (ImGui::CollapsingHeader("Window"))
	{
		App->window->DrawWindowInformationPanel();
	}
	if (ImGui::CollapsingHeader("Hardware Detection"))
	{
		App->hardware->DrawHardwareInformationPanel();
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

void ModuleGui::Print_Log(const char* text) {

	LOG_GLOBAL(text);
	log_list.push_back(text);

}

void ModuleGui::ShowLogWindow() {

	ImGui::SetNextWindowSize({ 200,400 });
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("Output Console", &Log_active, window_flags);
	for (int i = 0; i < log_list.size(); i++) {
		ImGui::Text(log_list.at(i));
	}
	ImGui::End();
}