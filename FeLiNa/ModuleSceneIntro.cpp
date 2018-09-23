#include "Application.h"
#include "ModuleSceneIntro.h"

#include <stdio.h>
#include <time.h>



ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{
}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG_GLOBAL("Loading Intro assets");
	bool ret = true;


	//Initial position camera.
	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	grid_plane->is_grid = true;

	//TEST MATGEOLIB WINDOW GUI

	sphere1 = new Sphere({ 0,0,0 }, 10);
	sphere2 = new Sphere({ 0,0,0 }, 10);

	capsule1 = new Capsule(LineSegment({ 0,0,0 }, {10,10,10}),10);
	capsule2 = new Capsule(LineSegment({ 0,0,0 }, { 10,10,10 }), 10);

	aabb1 = new AABB({ 0,0,0 }, {10,10,10});

	obb = new OBB(AABB({ 0,0,0 }, { 10,10,10 }));

	
	//SITO------------------------------------------------
	
	
	//ImGui
	 // Setup Dear ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer3D->context);
	ImGui_ImplOpenGL2_Init();

	// Setup style
	ImGui::StyleColorsDark();


	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG_GLOBAL("Unloading Intro scene");

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

update_status ModuleSceneIntro::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();

	return update_return;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	ImGui::ShowDemoWindow();

	ShowMainMenuBar();

	if (show_random_number_window)
	{
		ShowRandomNumberWindow();
	}

	if (show_MatGeo_window)
	{
		ShowMatGeoLibWindow();
	}


	return update_return;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	grid_plane->Render();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	if (close_program)
		update_return = UPDATE_STOP;

	return update_return;
}

void ModuleSceneIntro::ShowMainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			//MenuItem(const char* label, const char* shortcut, bool selected, bool enabled)
			if (ImGui::MenuItem("Close FeLiNa", NULL, false, true))
			{
				close_program = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window"))
		{
	
			if (ImGui::MenuItem("Random Number Generator", NULL, false, true))
			{
					show_random_number_window = !show_random_number_window;
			}

			if (ImGui::MenuItem("MathGeo window", NULL, false, true))
			{
				show_MatGeo_window = !show_MatGeo_window;
			}

			ImGui::EndMenu();
		}


		ImGui::EndMainMenuBar();
	}
}

void ModuleSceneIntro::ShowRandomNumberWindow()
{

	//SetNextWindowSize-> You can define the size of the next window that you create and add this size in Game/imgui.ini
		// else if you don't define the size ImGui assign lenght {32,35} pixels.
	ImGui::SetNextWindowSize({ 300,200 });


	//ImGuiWindowFlags are the flags when you use Begin() in a new window for customize an say to ImGui how is this window.
	//If you don't say for example: window_flags |= ImGuiWindowFlags_NoScrollbar the windows create have scrollbar. 
	//I think that's use to omit execution code in a run process, next class will ask to Ric. 
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;


	ImGui::Begin("Random Number Generator", false, window_flags);

	ImGui::Text("Generate random int between min to max");

	ImGui::InputInt("Min value", &min_value);
	ImGui::InputInt("Max value", &max_value);


	if (ImGui::Button("Generate int"))
	{
		if (min_value > max_value)
		{
			int tmp = min_value;

			min_value = max_value;
			max_value = tmp;
		}

		uint32_t bound;
		bound = max_value - min_value + 1;
		// See this: http://www.pcg-random.org/using-pcg-c.html for understand. 
		interger_random_generate = pcg32_boundedrand_r(&random_generator, bound);
		interger_random_generate -= abs(min_value);
	}
	ImGui::SameLine();
	ImGui::Text("Number: %i", interger_random_generate);

	ImGui::Text("Generate random float between 0.0 and 1.0");

	if (ImGui::Button("Generate float"))
	{
		// See this: http://www.pcg-random.org/using-pcg-c.html and http://mumble.net/~campbell/tmp/random_real.c for understand. 

		float_random_number = ldexp(pcg32_random_r(&random_generator), -32);

	}

	ImGui::SameLine();
	ImGui::Text("Number: %f", float_random_number);

	ImGui::End();
}


void ModuleSceneIntro::ShowMatGeoLibWindow()
{
	ImGui::SetNextWindowSize({ 500,900 });
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	//window_flags |= ImGuiWindowFlags_NoScrollbar;
	window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("MatGeolib Test collision", false, window_flags);

	ImGui::Text("Sphere 1");

	ImGui::SliderFloat("S1 X",&sphere1->pos.x, -100.0,100.0);
	ImGui::SliderFloat("S1 Y", &sphere1->pos.y, -100.0, 100.0);
	ImGui::SliderFloat("S1 Z", &sphere1->pos.z, -100.0, 100.0);
	ImGui::SliderFloat("S1 Radius", &sphere1->r, -100.0, 100.0);

	ImGui::Text("Sphere 2");

	ImGui::SliderFloat("S2 X", &sphere2->pos.x, -100.0, 100.0);
	ImGui::SliderFloat("S2 Y", &sphere2->pos.y, -100.0, 100.0);
	ImGui::SliderFloat("S2 Z", &sphere2->pos.z, -100.0, 100.0);
	ImGui::SliderFloat("S2 Radius", &sphere2->r, -100.0, 100.0);

	ImGui::Text("Capsule 1");

	ImGui::SliderFloat("C1 A.x", &capsule1->l.a.x, -100.0, 100.0);
	ImGui::SliderFloat("C1 A.y", &capsule1->l.a.y, -100.0, 100.0);
	ImGui::SliderFloat("C1 A.z", &capsule1->l.a.z, -100.0, 100.0);

	ImGui::SliderFloat("C1 B.x", &capsule1->l.b.x, -100.0, 100.0);
	ImGui::SliderFloat("C1 B.y", &capsule1->l.b.y, -100.0, 100.0);
	ImGui::SliderFloat("C1 B.z", &capsule1->l.b.z, -100.0, 100.0);

	ImGui::SliderFloat("C1 Radius", &capsule1->r, -100.0, 100.0);

	ImGui::Text("Capsule 2");

	ImGui::SliderFloat("C2 A.x", &capsule2->l.a.x, -100.0, 100.0);
	ImGui::SliderFloat("C2 A.y", &capsule2->l.a.y, -100.0, 100.0);
	ImGui::SliderFloat("C2 A.z", &capsule2->l.a.z, -100.0, 100.0);

	ImGui::SliderFloat("C2 B.x", &capsule2->l.b.x, -100.0, 100.0);
	ImGui::SliderFloat("C2 B.y", &capsule2->l.b.y, -100.0, 100.0);
	ImGui::SliderFloat("C2 B.z", &capsule2->l.b.z, -100.0, 100.0);

	ImGui::SliderFloat("C2 Radius", &capsule2->r, -100.0, 100.0);

	ImGui::Text("AABB");

	ImGui::SliderFloat("AABB A.x", &aabb1->minPoint.x, -100.0, 100.0);
	ImGui::SliderFloat("AABB A.y", &aabb1->minPoint.y, -100.0, 100.0);
	ImGui::SliderFloat("AABB A.z", &aabb1->minPoint.z, -100.0, 100.0);

	ImGui::SliderFloat("AABB B.x", &aabb1->maxPoint.x, -100.0, 100.0);
	ImGui::SliderFloat("AABB B.y", &aabb1->maxPoint.y, -100.0, 100.0);
	ImGui::SliderFloat("AABB B.z", &aabb1->maxPoint.z, -100.0, 100.0);

	ImGui::Text("OBB");

	ImGui::SliderFloat("OBB Pos x", &obb->pos.x, -100.0, 100.0);
	ImGui::SliderFloat("OBB Pos y", &obb->pos.y, -100.0, 100.0);
	ImGui::SliderFloat("AABB Pos z", &obb->pos.z, -100.0, 100.0);


	ImGui::Text("Sphere 1 collision with: ");
	if(sphere1->Contains(*sphere2))
		ImGui::Text("Sphere 2");
	else
		ImGui::Text("Not Sphere 2");

	if (sphere1->Contains(*capsule1))
		ImGui::Text("Capsule 1");
	else
		ImGui::Text("Not Capsule 1");

	if (sphere1->Contains(*capsule2))
		ImGui::Text("Capsule 2");
	else
		ImGui::Text("Not Sphere 2");

	if (sphere1->Contains(*aabb1))
		ImGui::Text("AABB");
	else
		ImGui::Text("Not AABB");

	if (sphere1->Contains(*obb))
		ImGui::Text("OBB");
	else
		ImGui::Text("Not OBB");

	ImGui::Text("Sphere 1 intersect with: ");
	if (sphere1->Intersects(*sphere2))
		ImGui::Text("Sphere 2");
	else
		ImGui::Text("Not Sphere 2");

	if (sphere1->Intersects(*capsule1))
		ImGui::Text("Capsule 1");
	else
		ImGui::Text("Not Capsule 1");

	if (sphere1->Intersects(*capsule2))
		ImGui::Text("Capsule 2");
	else
		ImGui::Text("Not Sphere 2");

	if (sphere1->Intersects(*aabb1))
		ImGui::Text("AABB");
	else
		ImGui::Text("Not AABB");

	if (sphere1->Intersects(*obb))
		ImGui::Text("OBB");
	else
		ImGui::Text("Not OBB");
	

	ImGui::End();
}