#include "Application.h"
#include "Globals.h"

Application::Application()
{
	hardware = new ModuleHardware(this,false);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	audio = new ModuleAudio(this, true);
	scene_intro = new ModuleSceneIntro(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	gui = new ModuleGui(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(hardware);
	// Scenes
	AddModule(scene_intro);
	AddModule(gui);
	// Renderer last!
	AddModule(renderer3D);
}

Application::~Application()
{

	for (std::list<Module*>::const_reverse_iterator it = list_modules.rbegin(); it != list_modules.rend(); ++it)
	{
		delete *it;
	}

}

bool Application::Init()
{
	bool ret = true;

	
	strcpy_s(app_name, "FeLina Engine");
	strcpy_s(organization, "UPC CITM");
	// Call Init() in all modules
	for (std::list<Module*>::const_iterator it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		ret = (*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG_GLOBAL("Application Start --------------");
	for (std::list<Module*>::const_iterator it = list_modules.begin(); it != list_modules.end() && ret; ++it)
	{
		ret = (*it)->Start();
	}
	
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	last_ms = ms_timer.ReadMs();

	if (!vsync) {

		double ms_cap = 0;

		if (FPS_cap > 0) {
			ms_cap = 1000 / FPS_cap;
		}

		else {
			ms_cap = 1000 / 60;
		}
		if (last_ms < ms_cap) {
			SDL_Delay(ms_cap - last_ms);
		}

	}

	last_ms = ms_timer.ReadMs();
	last_FPS = 1000.0 / last_ms;
	dt = 1 / last_FPS;
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	std::list<Module*>::const_iterator it = list_modules.begin();
	
	while(it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*it)->PreUpdate(dt);
		++it;
	}

	it = list_modules.begin();

	while(it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*it)->Update(dt);
		++it;
	}

	it = list_modules.begin();

	while(it != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*it)->PostUpdate(dt);
		++it;
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::const_reverse_iterator it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

float Application::GetMS()
{
	return last_ms;
}

float Application::GetFPS()
{
	return last_FPS;
}