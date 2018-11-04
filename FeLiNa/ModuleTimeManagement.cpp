#include "ModuleTimeManagement.h"


ModuleTimeManagement::ModuleTimeManagement(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	PauseGameClock();
}

ModuleTimeManagement::~ModuleTimeManagement()
{
	
}

bool ModuleTimeManagement::Start()
{
	
	
	return true;
}

bool ModuleTimeManagement::CleanUp()
{
	return true;
}

float ModuleTimeManagement::ReadGameClock() 
{
	return game_time;
}

float ModuleTimeManagement::ReadRealTimeClock() 
{
	return real_time;
}

void ModuleTimeManagement::PauseGameClock() 
{
	game_clock_active = false;

}

void ModuleTimeManagement::FinishUpdate() {

	Frame_count++;
	real_time += App->ms_timer.ReadMs()/1000;

	if (game_clock_active) {
		game_time += App->ms_timer.ReadMs()/1000;
		if (tick_selected) {
			game_clock_active = false;
			tick_selected = false;
		}
	}
	
}

void ModuleTimeManagement::StartGameTime() {

	game_clock_active = true;
	game_time = 0;

}

void ModuleTimeManagement::PlayGameTime() {

	game_clock_active = true;

}

