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
	mPlane* grid_plane;
	mCylinder* cylinder;
private:

	//ImGui
	bool show_demo_window = true;
	bool show_another_window = false;
	bool show_main_menu_bar = true;
	bool show_random_number_window = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	//Random Number Generator
	pcg32_random_t random_generator;
	
	double float_random_number = 0;

	int interger_random_generate = 0;
	int min_value = 0, max_value = 0;
};
