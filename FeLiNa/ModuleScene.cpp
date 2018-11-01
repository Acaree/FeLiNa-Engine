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
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentTexture.h"
#include "Quadtree.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "ComponentCamera.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Scene";
	root_object = new GameObject(nullptr);
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

	

	ComponentTransform* trans = new ComponentTransform(root_object);

	root_object->SetComponent(trans);

	root_object->AddChildren(App->camera->main_camera);

	App->mesh_import->LoadData("Assets\\BakerHouse.fbx");


	//TEST FRUSTUM
	quadtree = new QuadTree();
	math::AABB box;
	
	box.minPoint = { -50,-50,-50 };
	box.maxPoint = { 50,50,50 };

	quadtree->SetBoundary(box);

	return ret;
}

// Load assets
bool ModuleScene::CleanUp()
{
	LOG("Unloading Intro scene");

	quadtree->Clear();
	RELEASE(quadtree);

	delete grid_plane;
	grid_plane = nullptr;

	ImGui_ImplOpenGL2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_GL_DeleteContext(App->renderer3D->context);
	SDL_DestroyWindow(App->window->window);
	SDL_Quit();

	return true;
}

update_status ModuleScene::PreUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	FillStaticGameObjects();

	return update_return;
}


// Update
update_status ModuleScene::Update(float dt)
{

	update_status update_return = UPDATE_CONTINUE;
	
	ShowHierarchy();
	ShowInspector();

	root_object->Update(dt);
	return update_return;
}

update_status ModuleScene::PostUpdate(float dt)
{
	update_status update_return = UPDATE_CONTINUE;

	quadtree->DebugDraw();

	grid_plane->Render();

	return update_return;
}




void ModuleScene::ShowHierarchy()
{
	ImGui::SetNextWindowSize({300,500});
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

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
	ImGui::SetNextWindowSize({ 400,600 });
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("Inspector", &inspector_open, window_flags);

	GameObject* go = GetSelectedGameObject();

	if (go != nullptr)
	{
		go->ShowObjectInspector();
	}

	ImGui::End();

}

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
	quadtree->CollectIntersections(tmp_go, App->camera->dummy_frustum->frustum);

	for (uint i = 0; i < tmp_go.size(); ++i)
	{
		tmp_go[i]->SetActive(true);
	}

}