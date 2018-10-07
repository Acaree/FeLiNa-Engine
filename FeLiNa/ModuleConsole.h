#ifndef _MODULE_CONSOLE_
#define _MODULE_CONSOLE_

#include "Module.h"
#include "imgui-1.65/imgui.h"

class ModuleConsole : public Module
{
public:
	ModuleConsole(Application* app, bool start_enabled = true);
	~ModuleConsole();

	void Log_console(const char* text);
	void DrawConsole();

private:
	bool Log_active = false;
	ImGuiTextBuffer console_buffer;
	bool console_scroll = false;

	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);

};






#endif
