#include "Application.h"
#include "ModuleSceneIntro.h"

#include <stdio.h>
#include <time.h>



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{
}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG_GLOBAL("Loading Intro assets");
	bool ret = true;


	//Initial position camera.
	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	grid_plane->is_grid = true;
	
	
	//ImGui
	 // Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	// Setup style
	ImGui::StyleColorsDark();


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG_GLOBAL("Unloading Intro scene");

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_return;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;


	ShowMainMenuBar();

	if (show_random_number_window)
	{
		ShowRandomNumberWindow();
	}


	return update_return;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	grid_plane->Render();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	if (close_program)
		update_return = UPDATE_STOP;

	return update_return;
}

void ModuleSceneIntro::ShowMainMenuBar()
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
	
			if (ImGui::MenuItem("Random Number Generator", NULL, false, true))
			{
					show_random_number_window = !show_random_number_window;
			}
			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}
}

void ModuleSceneIntro::ShowRandomNumberWindow()
{

	//SetNextWindowSize-> You can define the size of the next window that you create and add this size in Game/imgui.ini
		// else if you don't define the size ImGui assign lenght {32,35} pixels.
	ImGui::SetNextWindowSize({ 300,200 });


	//ImGuiWindowFlags are the flags when you use Begin() in a new window for customize an say to ImGui how is this window.
	//If you don't say for example: window_flags |= ImGuiWindowFlags_NoScrollbar the windows create have scrollbar. 
	//I think that's use to omit execution code in a run process, next class will ask to Ric. 
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;


	ImGui::Begin("Random Number Generator", false, window_flags);

	ImGui::Text("Generate random int between min to max");

	ImGui::InputInt("Min value", &min_value);
	ImGui::InputInt("Max value", &max_value);


	if (ImGui::Button("Generate int"))
	{
		if (min_value > max_value)
		{
			int tmp = min_value;

			min_value = max_value;
			max_value = tmp;
		}

		uint32_t bound;
		bound = max_value - min_value + 1;
		// See this: http://www.pcg-random.org/using-pcg-c.html for understand. 
		interger_random_generate = pcg32_boundedrand_r(&random_generator, bound);
		interger_random_generate -= abs(min_value);
	}
	ImGui::SameLine();
	ImGui::Text("Number: %i", interger_random_generate);

	ImGui::Text("Generate random float between 0.0 and 1.0");

	if (ImGui::Button("Generate float"))
	{
		// See this: http://www.pcg-random.org/using-pcg-c.html and http://mumble.net/~campbell/tmp/random_real.c for understand. 

		float_random_number = ldexp(pcg32_random_r(&random_generator), -32);

	}

	ImGui::SameLine();
	ImGui::Text("Number: %f", float_random_number);

	ImGui::End();
}
