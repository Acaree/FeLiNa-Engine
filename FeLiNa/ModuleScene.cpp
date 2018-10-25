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

	root_object = new GameObject(nullptr);
	App->mesh_import->LoadData("Assets\\Hierarchy.FBX");
	
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
	
	ShowHierarchy();


	root_object->Update(dt);
	return update_return;
}

update_status ModuleScene::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	grid_plane->Render();

	return update_return;
}

void ModuleScene::CreateGameObject(GameObject* object)
{
	game_objects.push_back(object);
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

void ModuleScene::ShowHierarchy()
{
	ImGui::ShowDemoWindow();
	ImGui::SetNextWindowSize({300,500});
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("Hierarchy", &hierarchy_open ,window_flags);
	
	
	ImGuiTreeNodeFlags flags = 0;

	flags |= ImGuiTreeNodeFlags_DefaultOpen;

	
	for (uint i = 0; i < root_object->GetNumChildren(); ++i)
	{
		

		if (ImGui::TreeNodeEx(root_object->GetChild(i)->GetName()))
		{
			//TO Change
			root_object->GetChild(i)->ShowObjectHierarchy();
			//root_object->GetChild(i)->show = true;

			/*for (uint j = 0; j < root_object->GetChild(i)->GetNumChildren(); ++j)
			{
				root_object->GetChild(i)->ShowObjectHierarchy();
			}*/
			
			/*if (ImGui::TreeNodeEx(root_object->GetChild(i)->GetChild(0)->GetName()))
			{
				int x = 0;
				ImGui::TreePop();
			}

			if (ImGui::TreeNodeEx(root_object->GetChild(i)->GetChild(1)->GetName()))
			{
				int x = 0;
				ImGui::TreePop();
			}*/
			
			ImGui::TreePop();
		}

		
	}


	ImGui::End();
}
