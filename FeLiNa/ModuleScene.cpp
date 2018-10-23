#include "Application.h"
#include "ModuleScene.h"
#include "ModuleImport.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl2.h"
#include "GameObject.h"
#include "Component.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Scene";
}

ModuleScene::~ModuleScene()
{
}

// Load assets
bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//Initial position camera.
	App->camera->Move(float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(float3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	grid_plane->is_grid = true;

	App->mesh_import->LoadData("Assets\\BakerHouse.FBX");

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	delete grid_plane;
	grid_plane = nullptr;

	for (std::vector<GameObject*>::const_iterator it = game_objects.begin(); it != game_objects.end(); ++it)
	{
		(*it)->CleanUp();
	}

	game_objects.clear();

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

// Update
update_status ModuleScene::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;
	
	return update_return;
}

update_status ModuleScene::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	grid_plane->Render();

	return update_return;
}

void ModuleScene::CreateGameObject()
{
	game_objects.push_back(new GameObject());
}

void ModuleScene::CreateGameObject(GameObject* object)
{
	game_objects.push_back(object);
}

void ModuleScene::CreateGameObject(char* name)
{
	game_objects.push_back(new GameObject(name));
}

void ModuleScene::CreateGameObject(char* name, GameObject* parent, bool active)
{
	game_objects.push_back(new GameObject(name, parent,active));
}

bool ModuleScene::DeleteGameObject(GameObject* object)
{
	bool ret = false;

	if (game_objects.size() != 0)
	{

		for (std::vector<GameObject*>::const_iterator it = game_objects.begin(); it != game_objects.end(); ++it)
		{
			if ((*it) == object)
			{
				(*it)->CleanUp();
				game_objects.erase(it);

				ret = true;
			}

		}

		game_objects.shrink_to_fit();
	}

	return ret;

}

bool ModuleScene::DeleteGameObject(char* name)
{
	bool ret = false;

	if (game_objects.size() != 0)
	{

		for (std::vector<GameObject*>::const_iterator it = game_objects.begin(); it != game_objects.end(); ++it)
		{
			if (strcmp((*it)->GetName(),name) == 0)
			{
				(*it)->CleanUp();
				game_objects.erase(it);

				ret = true;
			}

		}

		game_objects.shrink_to_fit();
	}

	return ret;
}