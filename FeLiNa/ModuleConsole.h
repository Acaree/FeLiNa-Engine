#ifndef _MODULE_CONSOLE_
#define _MODULE_CONSOLE_

#ifndef GAME_MODE
#include "Module.h"
#include "ImGui/imgui.h"

struct ImGuiTextBuffer;

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

};
#endif

#endif
