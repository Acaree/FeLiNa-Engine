#ifndef _MODULE_TIME_MANAGEMENT_
#define _MODULE_TIME_MANAGEMENT_

#include "Module.h"
#include "Application.h"
#include "Timer.h"

#define SCALAR_TIME 0.0F

class ModuleTimeManagement: public Module
{
public:

	ModuleTimeManagement(Application* app, bool start_enabled = true);
	~ModuleTimeManagement();

	bool Start();
	bool CleanUp();

	float ReadGameClock() const;
	float ReadRealTimeClock() const;

	void PauseGameClock() const;
	


private:

	Timer* game_clock = nullptr;
	Timer* real_time_clock = nullptr;
};








#endif
