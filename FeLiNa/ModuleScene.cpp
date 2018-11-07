#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"

#ifndef GAME_MODE

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_opengl2.h"
#endif


#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "Quadtree.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "ComponentCamera.h"


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
	
	App->camera->LookAt(math::float3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	grid_plane->is_grid = true;

	
	root_object = new GameObject(nullptr);
	root_object->AddComponent(Component_Transform);

	

	root_object->AddChildren(App->camera->main_camera);
	App->camera->main_camera->SetParent(root_object);
	std::string output_file;

	App->importer_mesh->Import("BakerHouse.fbx","Assets/", output_file);
	
	
	//Mesh* new_mesh = App->importer_mesh->LoadFLN("Library/Meshes/Baker_House.felina");

	//App->importer_mesh->Import("Hierarchy.FBX", "Assets/", output_file);
	//Mesh* new_mesh = App->importer_mesh->LoadFLN("Library/Meshes/Box001.felina");

	/* Create a game object from .felina file
	GameObject* go = new GameObject(root_object);

	go->AddComponent(Component_Transform);
	go->AddComponent(Component_Mesh);

	go->mesh->SetMesh(new_mesh);

	App->renderer3D->meshes.push_back(go->mesh);

	go->AddBoundingBox(new_mesh);
	*/


	//std::string output_file;
	//Texture* tex = App->importer_material->Import("Baker_house.png","Assets/Textures/", output_file);


	//TEST FRUSTUM
	quadtree = new QuadTree();
	math::AABB box;
	
	box.minPoint = { -50,-50,-50 };
	box.maxPoint = { 50,50,50 };

	quadtree->SetBoundary(box);

	//FOR SERIALIZATION

	//s.LoadScene();
	//s.SaveScene();

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	RELEASE(grid_plane);
	RELEASE(quadtree);

	

	static_go.clear();

	root_object->CleanUp();

#ifndef GAME_MODE
	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

#endif

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	for (uint i = 0; i < root_object->GetNumChildren(); ++i)
	{
		SearchObjectsToDelete(root_object->GetChild(i));
	}


	//To Set and Update all static objects
	FillStaticGameObjects();

	return update_return;
}


// Update
update_status ModuleScene::Update(float dt)
{

	update_status update_return = UPDATE_CONTINUE;
	
	if (App->engine_states != ENGINE_STATES::ENGINE_STATE_GAME_MODE)
	{
#ifndef GAME_MODE
		ShowHierarchy();
		ShowInspector();
#endif
	}

	root_object->Update(dt);
	return update_return;
}

update_status ModuleScene::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	

	

	return update_return;
}

void ModuleScene::DrawScene()
{
	grid_plane->Render();
	//quadtree->DebugDraw();
}

#ifndef GAME_MODE

void ModuleScene::ShowHierarchy()
{
	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui::SetNextWindowPos(ImVec2(0, 17));
	ImGui::SetNextWindowSize(ImVec2( width / 4, height - 400 ));

	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	window_flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;

	ImGui::Begin("Hierarchy", &hierarchy_open ,window_flags);
	
	for (uint i = 0; i < root_object->GetNumChildren(); ++i)
	{
		ImGui::PushID(i);
		root_object->GetChild(i)->ShowObjectHierarchy();
		ImGui::PopID();
	}
	ImGui::End();
}

void ModuleScene::ShowInspector()
{
	static int width;
	static int height;
	SDL_GetWindowSize(App->window->window, &width, &height);

	ImGui::SetNextWindowPos(ImVec2(width-width/4, 17));
	ImGui::SetNextWindowSize(ImVec2(width / 4, height - 400));

	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;
	window_flags |= ImGuiWindowFlags_AlwaysVerticalScrollbar;
	window_flags |= ImGuiWindowFlags_AlwaysHorizontalScrollbar;

	ImGui::Begin("Inspector", &inspector_open, window_flags);

	GameObject* go = GetSelectedGameObject();

	if (go != nullptr)
	{
		go->ShowObjectInspector();
	}

	ImGui::End();

}
#endif

void ModuleScene::SetSelectedGameObject(GameObject* go)
{
	selected = go;
}

GameObject* ModuleScene::GetSelectedGameObject() const
{
	return selected;
}

void ModuleScene::FillStaticGameObjects()
{
	std::vector<GameObject*> tmp_go;

	if (need_update_quadtree)
	{
		//All GO THAt transform to dynamic remove
		for (uint i = 0; i < static_go.size(); ++i)
		{
			if (static_go[i]->static_object == false)
			{
				static_go.erase(static_go.begin()+i);

				if (i >= static_go.size())
					break;

			}
			
		}



		// Reset quatree
		if (static_go.size() < quadtree->size)
		{
			quadtree->Clear();
			math::AABB box;

			box.minPoint = { -50,-50,-50 };
			box.maxPoint = { 50,50,50 };

			quadtree->SetBoundary(box);
		}

		//Insert all static_go in quadtree
		for (uint i = quadtree->size; i < static_go.size(); ++i)
		{
			quadtree->Insert(static_go[i]);
		}

		need_update_quadtree = false;
	}


	//Set all static objects to  false
	for (uint i = 0; i < static_go.size(); ++i)
		static_go[i]->SetActive(false);

	//Get all go that collision with frustum
	quadtree->CollectIntersections(tmp_go, App->camera->main_camera->camera->frustum);

	for (uint i = 0; i < tmp_go.size(); ++i)
	{
		tmp_go[i]->SetActive(true);
	}

}

void ModuleScene::DeleteGameObjects(GameObject* go)
{
	if (go->IsSelected())
	{
		SetSelectedGameObject(nullptr);
	}

	for (uint i = 0; i < go->GetNumChildren(); ++i)
	{
		DeleteGameObjects(go->GetChild(i));
	}

	if (go->components.size() > 0)
	{
		go->DeleteAllComponents();
	}

	if (go->GetParent() != nullptr)
	{
		GameObject* parent = go->GetParent();

		parent->DeleteChildren(go);
	}
	else
	{
		//Necessary?? all game objects are parent of root if not have another parent
	}

}

void ModuleScene::SearchObjectsToDelete(GameObject* go)
{
	if (go->to_delete)
	{
		DeleteGameObjects(go);
	}

	for (uint i = 0; i < go->GetNumChildren(); ++i)
	{
		SearchObjectsToDelete(go->GetChild(i));
	}
}