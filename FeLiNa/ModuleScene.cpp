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
#include "ComponentMaterial.h"
#include "Quadtree.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "ComponentCamera.h"
#include "mmgr/mmgr.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Scene";
}

ModuleScene::~ModuleScene()
{
	RELEASE(root_object);
	
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

	game_camera = CreateMainCamera();

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
	UpdateStaticObjectsVector();

	return update_return;
}


// Update
update_status ModuleScene::Update(float dt)
{
	update_status update_return = UPDATE_CONTINUE;
	
	root_object->Update(dt);

	return update_return;
}


void ModuleScene::DrawScene()
{
	grid_plane->Render();
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


	if (ImGui::IsMouseClicked(1) && ImGui::IsWindowHovered())
	{
		ImGui::OpenPopup("Hierarchy Options");
	}

	if (ImGui::BeginPopupContextItem("Hierarchy Options"))
	{
		if (ImGui::MenuItem("Create Empty"))
		{
			GameObject* go = new GameObject(nullptr);
			go->AddComponent(Component_Transform);
			root_object->AddChildren(go);
		}
		ImGui::EndPopup();
	}

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


		if (ImGui::Button("Add Component", ImVec2(100, 25)))
		{
			ImGui::OpenPopup("New Component");
		}
		if (ImGui::BeginPopupContextItem("New Component"))
		{
			bool mesh_selectable = true;
			bool material_selectable = true;
			bool script_selectable = true;
			bool speed_selectable = true;

			if (go->mesh != nullptr)
				mesh_selectable = false;
			if (go->material != nullptr)
				material_selectable = false;
			if (go->script != nullptr)
				script_selectable = false;
			if (go->speed != nullptr) {
				speed_selectable = false;
			}

			if (ImGui::MenuItem("Add Component Mesh", NULL, false,mesh_selectable))
			{
				go->AddComponent(Component_Mesh);
			}

			if (ImGui::MenuItem("Add Component Material", NULL, false,material_selectable))
			{
				go->AddComponent(Component_Material);
			}

			if (ImGui::MenuItem("Add Component Script", NULL, false, script_selectable))
			{
				go->AddComponent(Component_Script);
			}

			if (ImGui::MenuItem("Add Component Speed", NULL, false, speed_selectable))
			{
				go->AddComponent(Component_Speed);
			}

			ImGui::EndPopup();
		}

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

void ModuleScene::UpdateStaticObjectsVector()
{
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
	go->childrens.clear();
	if (go->components.size() > 0)
	{
		go->DeleteAllComponents();
	}
	go->components.clear();

	if (go->GetParent() != nullptr)
	{
		GameObject* parent = go->GetParent();

		parent->DeleteChildren(go);
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

void ModuleScene::SetAllGameObjectActive(GameObject* go)
{
	go->SetActive(true);

	for (uint i = 0; i < go->GetNumChildren(); ++i)
		SetAllGameObjectActive(go->GetChild(i));
}

ComponentCamera* ModuleScene::CreateMainCamera()
{
	GameObject* obj_camera = new GameObject(nullptr);
	
	std::string name = "Main Camera";
	char* name_str = new char[name.size() + 1];
	strcpy(name_str, name.c_str());
	obj_camera->SetName(name_str);


	ComponentTransform* transform = (ComponentTransform*)obj_camera->AddComponent(Component_Transform);
	ComponentCamera* camera = (ComponentCamera*)obj_camera->AddComponent(Component_Camera);
	
	root_object->AddChildren(obj_camera);


	RELEASE_ARRAY(name_str);

	return camera;
}