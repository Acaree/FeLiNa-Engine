#ifndef _MODULE_INPUT_
#define _MODULE_INPUT_

#include "Module.h"
#include "Globals.h"

#define MAX_MOUSE_BUTTONS 5

enum KEY_STATE
{
	KEY_IDLE = 0,
	KEY_DOWN,
	KEY_REPEAT,
	KEY_UP
};

class ModuleInput : public Module
{
public:
	
	ModuleInput(Application* app, bool start_enabled = true);
	~ModuleInput();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	KEY_STATE GetKey(int id) const
	{
		return keyboard[id];
	}

	KEY_STATE GetMouseButton(int id) const
	{
		return mouse_buttons[id];
	}

	int GetMouseX() const
	{
		return mouse_x;
	}

	int GetMouseY() const
	{
		return mouse_y;
	}

	int GetMouseZ() const
	{
		return mouse_z;
	}

	int GetMouseXMotion() const
	{
		return mouse_x_motion;
	}

	int GetMouseYMotion() const
	{
		return mouse_y_motion;
	}
	void DrawInputConfiguration();

private:
	KEY_STATE* keyboard;
	KEY_STATE mouse_buttons[MAX_MOUSE_BUTTONS];
	int mouse_x;
	int mouse_y;
	int mouse_z;
	int mouse_x_motion;
	int mouse_y_motion;

	char shortcut_screenshot;
	char shortcut_gif;
	char shortcut_close_felina;

	char shortcut_wireframe;

	char shortcut_configuration;
	char shortcut_inspector;
	char shortcut_console;
	
	char shortcut_documentation;
	char shortcut_download;
	char shortcut_report_bug;
	char shortcut_about;
	
public:

	uint scancode_screenshot = 0;
	uint scancode_gif = 0;
	uint scancode_close_felina = 0;

	uint scancode_wireframe = 0;

	uint scancode_configuration = 0;
	uint scancode_inspector = 0;
	uint scancode_console = 0;

	uint scancode_documentation = 0;
	uint scancode_download = 0;
	uint scancode_report_bug = 0;
	uint scancode_about = 0;
	
};

#endif