#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ImGui/imgui_impl_sdl.h"
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

	//TO REVISE SET DEFAULT SHORTCUT
	shortcut_screenshot = '1';
	shortcut_gif = 'Q';
	shortcut_close_felina = '0';

	shortcut_wireframe = '2';
	shortcut_configuration = '3';
	shortcut_inspector = '4';
	shortcut_console = '5';

	shortcut_documentation = '6';
	shortcut_download = '7';
	shortcut_report_bug = '8';
	shortcut_about = '9';

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
		ImGui_ImplSDL2_ProcessEvent(&e);
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
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
			}

			break;

			

			case SDL_DROPFILE:
			{
				
				char* dropped_filedir;
				dropped_filedir = e.drop.file;
				// Shows directory of dropped file
				//SDL_ShowSimpleMessageBox(
					//SDL_MESSAGEBOX_INFORMATION,
					//"File dropped on window",
					//dropped_filedir,
					//App->window->window
				//);

				std::string path(dropped_filedir);

				

				if (path.substr(path.find_last_of(".")) == ".png" || path.substr(path.find_last_of(".")) == ".PNG" || path.substr(path.find_last_of(".")) == ".jpg" || path.substr(path.find_last_of(".")) == ".JPG" || path.substr(path.find_last_of(".")) == ".dds"  || path.substr(path.find_last_of(".")) == ".DDS") {

					if (App->renderer3D->data.size() > 0) {
						App->renderer3D->AssignTexture(dropped_filedir);
					}

				}
				// TO REVISION
				else if (path.substr(path.find_last_of(".")) == ".fbx" || path.substr(path.find_last_of(".")) == ".FBX") {
					App->renderer3D->DeleteAllDataMesh();
					App->mesh_import->LoadData(dropped_filedir);
					App->camera->FocusToCenterObject();
					App->gui->inspector_open = true;
				}

				SDL_free(dropped_filedir);
				
			}

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

update_status ModuleInput::PostUpdate(float dt) {

	update_status update_return = UPDATE_CONTINUE;

	last_update_ms = module_timer.ReadMs();
	module_timer.Start();

	return update_return;

}

void ModuleInput::DrawInputConfiguration()
{
	ImGui::Text("Shortcuts Menu: ");
	ImGui::Separator();

	if (ImGui::InputText("Screenshot ##1", &shortcut_screenshot, 2))
	{
	
	  SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_screenshot));

	}
	
	if (ImGui::InputText("Gif", &shortcut_gif, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_gif));

	}
	/*if (ImGui::InputText("Close FeLiNa", &shortcut_close_felina, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_close_felina));

	}

	ImGui::Text("Shortcuts Mode: ");
	ImGui::Separator();

	if (ImGui::InputText("Wireframe", &shortcut_wireframe, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_wireframe));

	}

	ImGui::Text("Shortcuts Windows: ");
	ImGui::Separator();

	if (ImGui::InputText("Configuration", &shortcut_configuration, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_configuration));

	}
	if (ImGui::InputText("Inspector", &shortcut_inspector, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_inspector));

	}
	if (ImGui::InputText("Console", &shortcut_console, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_console));

	}

	ImGui::Text("Shortcuts Help: ");
	ImGui::Separator();

	if (ImGui::InputText("Documentation",&shortcut_documentation, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_documentation));

	}
	if (ImGui::InputText("Download", &shortcut_download, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_download));

	}
	if (ImGui::InputText("Report a bug", &shortcut_report_bug, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_report_bug));

	}
	if (ImGui::InputText("About", &shortcut_about, 2))
	{

		SDL_GetScancodeFromKey(SDL_GetKeyFromName(&shortcut_about));

	}*/
}