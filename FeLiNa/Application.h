#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGui.h"
#include "ModuleHardware.h"

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	ModuleSceneIntro* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGui* gui;
	ModuleHardware* hardware;

	char app_name[20];
	char organization[20];

	int FPS_cap = 60;

	bool vsync = false;

private:

	PerfTimer	ms_timer;
	float	dt = 0.0f;
	double last_FPS = 0.0f;
	double last_ms = 0.0f;
	std::list<Module*>list_modules;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	float GetMS();
	float GetFPS();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};