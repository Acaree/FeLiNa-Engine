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

	float ReadGameClock();
	float ReadRealTimeClock();

	void StartGameTime();
	
	void PlayGameTime();

	void PauseGameClock();

	void FinishUpdate();
	
	float Real_Time_Delta_time = 0.0f;


private:

	Timer game_clock;
	Timer real_time_clock;
	float Time_scale = 1.0f;
	uint Frame_count = 0;

	float Real_time_start_frame = 0.0f;
};








#endif
