#ifndef _MODULEGUI_H
#define _MODULEGUI_H

#include "Module.h"


class ModuleGui : public Module
{
public:

	ModuleGui(Application* app, bool start_enabled = true);
	~ModuleGui();

	bool Start();
	bool Awake(JSON_Object* config);
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

private:
	bool About_active = false;
	
	void ShowMainMenuBar();
	void ShowConfigurationWindow();
	void ShowAboutWindow();

public:

	bool close_program = false;
	bool open_configuration = false;
	bool open_render_configuration = false;
	bool open_console = false;
	bool need_screenshoot = false; //TO REVISION
	bool inspector_open = false;
};


#endif
