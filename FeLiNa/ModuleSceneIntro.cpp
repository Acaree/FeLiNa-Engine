#include "Application.h"
#include "ModuleSceneIntro.h"





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



	return update_return;
}

// Update
update_status ModuleSceneIntro::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	glLineWidth(2.0f);
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);    
	glVertex3f(1.0f, 1.0f, -1.0f);    
	glVertex3f(-1.0f, 1.0f, -1.0f);   
	glVertex3f(-1.0f, 1.0f, 1.0f);    
	glVertex3f(1.0f, 1.0f, 1.0f);   
	glColor3f(1.0f, 0.5f, 0.0f);    
	glVertex3f(1.0f, -1.0f, 1.0f);    
	glVertex3f(-1.0f, -1.0f, 1.0f);    
	glVertex3f(-1.0f, -1.0f, -1.0f);    
	glVertex3f(1.0f, -1.0f, -1.0f);    
	glColor3f(1.0f, 0.0f, 0.0f);       
	glVertex3f(1.0f, 1.0f, 1.0f);    
	glVertex3f(-1.0f, 1.0f, 1.0f);    
	glVertex3f(-1.0f, -1.0f, 1.0f);   
	glVertex3f(1.0f, -1.0f, 1.0f);    
	glColor3f(1.0f, 1.0f, 0.0f);    
	glVertex3f(1.0f, -1.0f, -1.0f);    
	glVertex3f(-1.0f, -1.0f, -1.0f);    
	glVertex3f(-1.0f, 1.0f, -1.0f);    
	glVertex3f(1.0f, 1.0f, -1.0f);    
	glColor3f(0.0f, 0.0f, 1.0f);    
	glVertex3f(-1.0f, 1.0f, 1.0f);    
	glVertex3f(-1.0f, 1.0f, -1.0f);    
	glVertex3f(-1.0f, -1.0f, -1.0f);    
	glVertex3f(-1.0f, -1.0f, 1.0f);    
	glColor3f(1.0f, 0.0f, 1.0f);    
	glVertex3f(1.0f, 1.0f, -1.0f);    
	glVertex3f(1.0f, 1.0f, 1.0f);    
	glVertex3f(1.0f, -1.0f, 1.0f);    
	glVertex3f(1.0f, -1.0f, -1.0f);    
	glEnd();


	return update_return;
}

update_status ModuleSceneIntro::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	grid_plane->Render();

	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	return update_return;
}

