#include "ModuleTimeManagement.h"
#include "mmgr/mmgr.h"

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
	return game_time/1000;
}

float ModuleTimeManagement::ReadRealTimeClock() 
{
	return real_time/1000;
}

void ModuleTimeManagement::PauseGameClock() 
{
	game_clock_active = false;

}

void ModuleTimeManagement::FinishUpdate() {

	Frame_count++;
	real_time += App->ms_timer.ReadMs();

	if (game_clock_active) {
		game_time += App->ms_timer.ReadMs() * time_multiplier;
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

