#include "ModuleGui.h"
#include "Application.h"


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


		if (ImGui::BeginMenu("Links"))
		{

			if (ImGui::MenuItem("Github repository", NULL, false, true))
			{
				ShellExecute(NULL, "open", "https://github.com/Acaree/FeLiNa-Engine", NULL, NULL, SW_SHOWNORMAL);
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
	window_flags |= ImGuiWindowFlags_NoScrollbar;
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
		char title[25];

		sprintf_s(title, 25, "Framerate %.1f", vector_fps[vector_fps.size() - 1]);
		ImGui::PlotHistogram("##framerate", &vector_fps[0], vector_fps.size(),0,title,0.0f,100.0f,ImVec2(310,100));

		sprintf_s(title, 25, "Milliseconds %.1f", vector_ms[vector_ms.size() - 1]);
		ImGui::PlotHistogram("##milliseconds", &vector_ms[0], vector_ms.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
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

		//NEES SAVE/LOAD TO JSON
		if (ImGui::Checkbox("Reasizable", &App->window->reasizable))
		{
		
		}

		if (ImGui::Checkbox("Bordeless", &App->window->bordeless))
		{
				SDL_SetWindowBordered(App->window->window,(SDL_bool)!App->window->bordeless);
		}

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

	}

	ImGui::End();
}
