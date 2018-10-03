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
	console = new ModuleConsole(this);
	fbx = new ModuleFBX(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	AddModule(hardware);
	AddModule(console);
	AddModule(fbx);
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

	if (need_save)
		Save();

	if (need_load)
		Load();

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

void Application::Save()
{
	Log_app("Saving State....");

	JSON_Value* root = json_parse_file("data.json");

	if (root != nullptr)
	{
		JSON_Object* data = json_value_get_object(root);

		for (std::list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); ++item)
		{
			JSON_Object* module_to_save = json_object_get_object(data, (*item)->GetName());
			(*item)->SaveState(module_to_save);
		}

		json_serialize_to_file_pretty(root, "data.json");
		json_value_free(root);

		Log_app("Saving succesful");
	}
	else
		Log_app("Save failed: can't find root");

	need_save = false;
}

void Application::Load()
{
	Log_app("Loading State....");

	JSON_Value* root = json_parse_file("data.json");

	if (root != nullptr)
	{
		JSON_Object* data = json_value_get_object(root);

		for (std::list<Module*>::const_iterator item = list_modules.begin(); item != list_modules.end(); ++item)
		{
			JSON_Object* module_to_save = json_object_get_object(data, (*item)->GetName());
			(*item)->LoadState(module_to_save);
		}

		json_value_free(root);

		Log_app("Load succesful");
	}
	else
		Log_app("Load failed: can't find root");

	need_load = false;
}


void Application::DrawApplicationInformationPanel()
{
	ImGui::InputText("App name", app_name, 20);
		
	ImGui::InputText("Organization", organization, 20);
	if (vector_fps.size() != 100)
	{
		vector_fps.push_back(GetFPS());
		vector_ms.push_back(GetMS());
	}
	else
	{
		vector_fps.erase(vector_fps.begin());
		vector_fps.push_back(GetFPS());

		vector_ms.erase(vector_ms.begin());
		vector_ms.push_back(GetMS());
	}
	char title[30];

	ImGui::SliderInt("FPS", &FPS_cap, 0, 120);

	ImGui::Checkbox("Vsync", &vsync);

	sprintf_s(title, 25, "Framerate %.1f", vector_fps[vector_fps.size() - 1]);
	ImGui::PlotHistogram("##framerate", &vector_fps[0], vector_fps.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sprintf_s(title, 25, "Milliseconds %.1f", vector_ms[vector_ms.size() - 1]);
	ImGui::PlotHistogram("##milliseconds", &vector_ms[0], vector_ms.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));

	sMStats stats = m_getMemoryStatistics();

	if (vector_memory.size() != 100)
	{
		vector_memory.push_back((int)stats.accumulatedActualMemory);
	}
	else
	{
		vector_memory.erase(vector_memory.begin());
		vector_memory.push_back((int)stats.accumulatedActualMemory);
	}

	sprintf_s(title, 25, "Memory Consumption ");
	ImGui::PlotHistogram("##Memory Consumption", &vector_memory[0], vector_memory.size(), 0, title, 0.0f, 1000000, ImVec2(310, 100));

	ImGui::Text("Total Reported Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.totalReportedMemory);

	ImGui::Text("Total Actual Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedActualMemory);

	ImGui::Text("Peak Reported Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.peakReportedMemory);

	ImGui::Text("Peak Actual Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.peakActualMemory);

	ImGui::Text("Accumulated Reported Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedReportedMemory);

	ImGui::Text("Accumulated Actual Mem:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedActualMemory);

	ImGui::Text("Accumulated Alloc Unit Count:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.accumulatedAllocUnitCount);

	ImGui::Text("Total Alloc Unit Count:");
	ImGui::SameLine();
	ImGui::Text("%i", stats.totalAllocUnitCount);

	ImGui::Text("Peak Alloc Unit Count");
	ImGui::SameLine();
	ImGui::Text("%i", stats.peakAllocUnitCount);
}

void Application::Log_app(const char * text)
{
	console->Log_console(text);
}

void Application::NeedSave()
{
	need_save = true;
}

void Application::NeedLoad()
{
	need_load = true;
}