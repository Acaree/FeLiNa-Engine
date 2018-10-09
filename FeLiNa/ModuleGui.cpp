#include "ModuleGui.h"
#include "Application.h"
#include "mmgr/mmgr.h"
#include "ModuleHardware.h"
#include "ModuleSceneIntro.h"



ModuleGui::ModuleGui(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Gui";
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
	module_timer.Start();

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_return;
}

update_status ModuleGui::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ShowMainMenuBar();

	if(open_configuration)
		ShowConfigurationWindow();

	if (open_render_configuration)
		App->renderer3D->DrawCheckBoxEdgeGLPanel();

	if (open_console)
		App->console->DrawConsole();

	if (inspector_open)
		App->renderer3D->DrawMeshInformation();
	

	if (About_active) {
		ShowAboutWindow();
	}
	
	return update_return;
}

update_status ModuleGui::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;
	
	

	if (close_program)
		update_return = UPDATE_STOP;

	last_update_ms = module_timer.ReadMs();
	module_timer.Start();

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

			if (ImGui::MenuItem("Take Screenshot", NULL, false, true))
			{
				need_screenshoot = true;
			}

			if (ImGui::MenuItem("Close FeLiNa", NULL, false, true))
			{
				close_program = true;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Mode"))
		{

			if (ImGui::MenuItem("Wireframe", NULL, App->scene_intro->grid_plane->wire, true))
			{
				App->scene_intro->grid_plane->wire = !App->scene_intro->grid_plane->wire;
			}

			if (ImGui::MenuItem("GL Options", NULL, open_render_configuration, true))
			{
				open_render_configuration = !open_render_configuration;
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Configuration", NULL, open_configuration, true))
			{
				open_configuration = !open_configuration;
			}

			if (ImGui::MenuItem("Inspector", NULL, inspector_open, true))
			{
				inspector_open = !inspector_open;
			}

			if (ImGui::MenuItem("Console", NULL, open_console, true))
			{
				open_console = !open_console;
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
				About_active = !About_active;	
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

	if (ImGui::CollapsingHeader("Modules Update Time"))
	{
		App->DrawModulesTime();
	}


	ImGui::End();
}


void ModuleGui::ShowAboutWindow() {

	
	ImGuiWindowFlags window_flags = 0;

	
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_NoMove;

	ImGui::Begin("About",&About_active, window_flags);

	ImGui::Text("FeLiNa Engine.");
	ImGui::Separator();
	ImGui::Text("This is a 3D game engine developed by two students\n from CITM-UPC Terrasa.");
	ImGui::Spacing();
	ImGui::Text("Authors:");
	if(ImGui::Button("Alfonso Sanchez-Cortes"))
		ShellExecute(NULL, "open", "https://github.com/Siitoo", NULL, NULL, SW_SHOWNORMAL);
	if (ImGui::Button("Alex Campamar"))
		ShellExecute(NULL, "open", "https://github.com/Acaree", NULL, NULL, SW_SHOWNORMAL);
	ImGui::Separator();
	ImGui::Text("Libraries:");

	SDL_version version;
	SDL_GetVersion(&version);

	ImGui::Text("ImGuI, OpenGL %s, \nSDL %i.%i.%i, MathGeoLib, PCG, Parson, MMGR", glGetString(GL_VERSION), version.major, version.minor, version.patch);
	ImGui::Separator();

	ImGui::Text("MIT License Copyright(c) 2018 FeLiNa \n");
	ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy");
	ImGui::Text("of this software and associated documentation files(the %cSoftware%c), to deal", 34, 34);
	ImGui::Text("in the Software without restriction, including without limitation the rights");
	ImGui::Text("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell");
	ImGui::Text("copies of the Software, and to permit persons to whom the Software is");
	ImGui::Text("furnished to do so, subject to the following conditions :");
	ImGui::Text("The above copyright notice and this permission notice shall be included in all");
	ImGui::Text("copies or substantial portions of the Software.");
	ImGui::Text("THE SOFTWARE IS PROVIDED %cAS IS%c, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR", 34, 34);
	ImGui::Text("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,");
	ImGui::Text("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE");
	ImGui::Text("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER");
	ImGui::Text("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,");
	ImGui::Text("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE");
	ImGui::Text("SOFTWARE.");

	ImGui::End();

}



