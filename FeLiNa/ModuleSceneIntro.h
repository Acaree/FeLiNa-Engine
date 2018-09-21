#pragma once

#include "Module.h"
#include "Primitive.h"
#include "imgui-1.65/imgui.h"
#include "imgui-1.65/imgui_impl_sdl.h"
#include "imgui-1.65/imgui_impl_opengl2.h"
#include "SDL/include/SDL_opengl.h"

//Test
#include "Pcg/pcg_variants.h"


class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	Plane* grid_plane;

private:

	//ImGui
	bool show_demo_window = true;
	bool show_another_window = false;
	bool show_main_menu_bar = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};
