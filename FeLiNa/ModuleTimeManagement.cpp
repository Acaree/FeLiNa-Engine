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
	real_time_clock.Start();
	
	return true;
}

bool ModuleTimeManagement::CleanUp()
{
	return true;
}

float ModuleTimeManagement::ReadGameClock() 
{
	return game_clock.ReadSec();
}

float ModuleTimeManagement::ReadRealTimeClock() 
{
	return real_time_clock.Read();
}

void ModuleTimeManagement::PauseGameClock() 
{
	game_clock.Pause();

}

void ModuleTimeManagement::FinishUpdate() {

	Frame_count++;
	Real_Time_Delta_time = real_time_clock.ReadSec() - Real_time_start_frame;
	Real_time_start_frame = real_time_clock.ReadSec();
}

void ModuleTimeManagement::StartGameTime() {

	game_clock.Start();

}

void ModuleTimeManagement::PlayGameTime() {

	game_clock.Play();

}

