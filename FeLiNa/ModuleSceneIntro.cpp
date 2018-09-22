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
	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	
	//cylinder = new mCylinder(1.0f, 1.0f);

	//ImGui
	 // Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

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

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	//ImGui

	/*
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);


		}*/

		// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	//MAIN BAR----------------------------------------------------------------------------
	if (show_main_menu_bar)
	{

		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("Menu"))
			{
				//MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
				if (ImGui::MenuItem("Close FeLiNa", NULL, false, true))
				{
					update_return = UPDATE_STOP;
				}
				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
	//Sito--------------------------------------------------------------------------------------

	//Random number generator with ImGui window-------------------------------------------------

	if (show_random_number_window)
	{
		//SetNextWindowSize-> You can define the size of the next window that you create and add this size in Game/imgui.ini
		// else if you don't define the size ImGui assign lenght {32,35} pixels.
		ImGui::SetNextWindowSize({300,200});


		//ImGuiWindowFlags are the flags when you use Begin() in a new window for customize an say to ImGui how is this window.
		//If you don't say for example: window_flags |= ImGuiWindowFlags_NoScrollbar the windows create have scrollbar. 
		//I think that's use to omit execution code in a run process, next class will ask to Ric. 
		ImGuiWindowFlags window_flags = 0;

		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
		

		ImGui::Begin("Random Number Generator",false, window_flags);
		
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
			interger_random_generate = pcg32_boundedrand_r(&random_generator,bound);
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

	//Sito--------------------------------------------------------------------------------------

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{

		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}


	//Render plane
	grid_plane->Render();
	//cylinder->Render();

	// Rendering
	ImGui::Render();

	//glUseProgram(0); // You may want this if using this code in an OpenGL 3+ context where shaders may be bound
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	//SDL_GL_SwapWindow(App->window->window);





	return update_return;
}