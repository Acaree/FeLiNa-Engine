#ifndef _MODULEGUI_H
#define _MODULEGUI_H

#include "Module.h"
#include "Globals.h"

#include "imgui-1.65/imgui.h"
#include "imgui-1.65/imgui_impl_sdl.h"
#include "imgui-1.65/imgui_impl_opengl2.h"

#include <vector>

class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void Print_Log(const char* text);

private:
	bool About_active = false;
	
	//Log console
	bool Log_active = false;
	std::vector<const char*> log_list;

	void ShowMainMenuBar();
	void ShowConfigurationWindow();
	void ShowAboutWindow();
	void ShowLogWindow();

public:
	
	bool close_program = false;
	bool open_configuration = true;
};


#endif
