#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
}

// Destructor
ModuleWindow::~ModuleWindow()
{
	name = "Window";
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
		width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;
		flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE); 
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); 
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24); 
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 2); //BUG: it should be 8 but if not 2, game crashes (conflicts with mathgeolib)
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); 
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->app_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

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

	brightness = SDL_GetWindowBrightness(window);
	return ret;
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

void ModuleWindow::SetTitle(const char* title)
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

	if (ImGui::SliderInt("Width", &width, 1, 2000) || ImGui::SliderInt("Height", &height, 1, 2000))
	{
		SDL_SetWindowSize(window, width, height);
		SDL_UpdateWindowSurface(window);
	}
	ImGui::Spacing();

	//�Refresh rate?

	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		if (fullscreen)
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		else
			SDL_SetWindowFullscreen(window, flags);
	}
	ImGui::SameLine();

	//�Reasizable?

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




}
