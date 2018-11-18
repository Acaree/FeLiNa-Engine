#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool Awake(JSON_Object* config);
	bool CleanUp();

	void SetTitle(const char* title) const;
	void DrawWindowInformationPanel();
	void SaveState(JSON_Object* config);
public:

	
	SDL_Window* window = nullptr;
	Uint32 flags;
	SDL_Surface* screen_surface;

	int width = 1280;
	int height = 960;

private:


	float brightness = 1;
	int refresh_rate = 0;
	bool fullscreen = false, reasizable = false, bordeless=false, fullscreen_desktop= false;
};

#endif // __ModuleWindow_H__