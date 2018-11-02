#include "ModuleTimeManagement.h"


ModuleTimeManagement::ModuleTimeManagement(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	game_clock = new Timer();
	real_time_clock = new Timer();
}

ModuleTimeManagement::~ModuleTimeManagement()
{
	RELEASE(game_clock);
	RELEASE(real_time_clock);
}

bool ModuleTimeManagement::Start()
{
	game_clock->Start();
	real_time_clock->Start();
	
	return true;
}

bool ModuleTimeManagement::CleanUp()
{
	RELEASE(game_clock);
	RELEASE(real_time_clock);

	return true;
}

float ModuleTimeManagement::ReadGameClock() const
{
	return game_clock->ReadSec();
}

float ModuleTimeManagement::ReadRealTimeClock() const
{
	return real_time_clock->Read();
}

void ModuleTimeManagement::PauseGameClock() const
{
	game_clock->Pause();
}