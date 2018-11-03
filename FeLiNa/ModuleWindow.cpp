#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ImGui/imgui.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
	name = "Window";
}

// Destructor
ModuleWindow::~ModuleWindow()
{
	RELEASE(window);
	RELEASE(screen_surface);
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		/*width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;*/
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
		
		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); 
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 2); //BUG: it should be 8 but if not 2, game crashes (conflicts with mathgeolib)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(reasizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(bordeless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->app_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		SDL_SetWindowBrightness(window, brightness);
		SDL_UpdateWindowSurface(window);
		
		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	
	return ret;
}

bool ModuleWindow::Awake(JSON_Object* config)
{
	bool ret = true;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		ret = false;
	else
	{
		width = json_object_get_number(config, "Width");
		height = json_object_get_number(config, "Height");
		brightness = json_object_get_number(config, "Brightness");
		fullscreen = json_object_get_boolean(config, "Fullscreen");
		reasizable = json_object_get_boolean(config, "Reasizable");
		bordeless = json_object_get_boolean(config, "Bordeless");
		fullscreen_desktop = json_object_get_boolean(config, "Fullscreen desktop");

		ret = true;
	}

	return ret;
}

void ModuleWindow::SaveState(JSON_Object* config)
{
	json_object_set_number(config, "Width", width);
	json_object_set_number(config, "Height", height);
	json_object_set_number(config, "Brightness", brightness);
	json_object_set_boolean(config, "Fullscreen", fullscreen);
	json_object_set_boolean(config, "Reasizable", reasizable);
	json_object_set_boolean(config, "Bordeless", bordeless);
	json_object_set_boolean(config, "Fullscreen desktop", fullscreen_desktop);
}


// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");
	SDL_SetWindowBrightness(window, 1.0);
	SDL_UpdateWindowSurface(window);
	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title) const
{
	SDL_SetWindowTitle(window, title);
}

void ModuleWindow::DrawWindowInformationPanel()
{
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
	{
		SDL_SetWindowBrightness(window, brightness);
		SDL_UpdateWindowSurface(window);
	}

	if (ImGui::SliderInt("Width", &width, 1, 2000))
	{
		SDL_SetWindowSize(window, width, height);
		SDL_UpdateWindowSurface(window);
	}

	if (ImGui::SliderInt("Height", &height, 1, 2000))
	{
		SDL_SetWindowSize(window, width, height);
		SDL_UpdateWindowSurface(window);
	}
	ImGui::Spacing();

	//¿Refresh rate?

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		else
			SDL_SetWindowFullscreen(window, flags);
	}
	ImGui::SameLine();

	if (ImGui::Checkbox("Bordeless", &bordeless))
	{
		SDL_SetWindowBordered(window, (SDL_bool)!bordeless);
	}
	ImGui::SameLine();

	if (ImGui::Checkbox("Fullscreen Desktop", &fullscreen_desktop))
	{
		if (fullscreen_desktop)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			SDL_SetWindowFullscreen(window, flags);
	}

	ImGui::Text("Restart to apply changes:");
	ImGui::SameLine();
	if (ImGui::Checkbox("Resizable", &reasizable));

}
