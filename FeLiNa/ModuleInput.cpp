#include "ModuleRenderer3D.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleFileSystem.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "ModuleGui.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include <algorithm>
#include "mmgr/mmgr.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Input";

	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Init()
{
	LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);


	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}


// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	module_timer.Start();

	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= SCREEN_SIZE;
	mouse_y /= SCREEN_SIZE;
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;

	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
#ifndef GAME_MODE
		ImGui_ImplSDL2_ProcessEvent(&e);
#endif
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:

				mouse_z = e.wheel.y;
				break;

			case SDL_MOUSEMOTION:
			
				mouse_x = e.motion.x / SCREEN_SIZE;
				mouse_y = e.motion.y / SCREEN_SIZE;

				mouse_x_motion = e.motion.xrel / SCREEN_SIZE;
				mouse_y_motion = e.motion.yrel / SCREEN_SIZE;
				break;
			
			case SDL_QUIT:
			
				quit = true;
				break;
			
			case SDL_WINDOWEVENT:
			
				if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);

				break;
#ifndef GAME_MODE
			case SDL_DROPFILE:
			{

				char* dropped_filedir = App->fs->MoveFileToAssets(e.drop.file);
				
				uint uid = App->resource_manager->ImportFile(dropped_filedir);
				
				Resource* resource = App->resource_manager->Get(uid);

				std::string path = dropped_filedir;
				path = path.substr(0, path.find_last_of("/")+1);

				std::string file_name = dropped_filedir;
				file_name = file_name.substr(file_name.find_last_of("/")+1, file_name.size());

				std::string output_file;

				switch (resource->type)
				{
				case RESOURCE_TYPE::RESOURCE_MESH:
				{
					MeshSettings* default_settings = new MeshSettings();
					App->importer_mesh->Import(file_name.c_str(), path.c_str(),output_file,default_settings);
					RELEASE(default_settings);
				}
					break;
				case RESOURCE_TYPE::RESOURCE_MATERIAL:
					MaterialSettings* default_settings = new MaterialSettings();
					App->importer_material->Import(dropped_filedir, output_file, default_settings);
					break;
				}
				//Texture* tex = App->importer_material->Import(dropped_filedir, output_file);
				//App->resource_manager->ImportFile("Assets/Hierarchy.FBX");

				SDL_free(&dropped_filedir);

				break;
			}
#endif
			default: 
				break;
		}

	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

#ifndef GAME_MODE




void ModuleInput::DrawInputConfiguration() const
{
	ImGui::Text("Mouse Position: ");
	ImGui::Text("x: %i", mouse_x);
	ImGui::SameLine();
	ImGui::Text("y: %i", mouse_y);
	ImGui::Separator();
	ImGui::Text("Mouse Motion: ");
	ImGui::Text("x: %i", mouse_x_motion);
	ImGui::SameLine();
	ImGui::Text("y: %i", mouse_y_motion);

}

#endif