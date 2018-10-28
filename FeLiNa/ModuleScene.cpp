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
	App->camera->Move(math::float3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(math::float3(0, 0, 0));

	//Create Axis Plane 
	grid_plane = new mPlane(0, 1, 0, 0);
	grid_plane->axis = true;
	grid_plane->is_grid = true;

	root_object = new GameObject(nullptr);

	ComponentTransform* trans = new ComponentTransform(root_object);

	root_object->SetComponent(trans);

	App->mesh_import->LoadData("Assets\\Street environment_V01.FBX");
	
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
	ShowInspector();

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
	ImGui::SetNextWindowSize({ 300,500 });
	ImGuiWindowFlags window_flags = 0;

	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoFocusOnAppearing;

	ImGui::Begin("Inspector", &inspector_open, window_flags);


	//MEMORY LEAK?

	GameObject* go = GetSelectedGameObject();

	ComponentTransform* transform = nullptr;
	ComponentMesh* mesh = nullptr;
	ComponentTexture* tex = nullptr;

	if (go != nullptr) {

		transform = (ComponentTransform*)go->GetComponent(Component_Transform);
		mesh = (ComponentMesh*)go->GetComponent(Component_Mesh);
		tex = (ComponentTexture*)go->GetComponent(Component_Material);
	}

	if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {

		if (transform != nullptr) {

			if (ImGui::CollapsingHeader("Position:")) {

				math::float3 pos = transform->GetPosition();

				char Pos_x_c[16] = {};

				sprintf_s(Pos_x_c, "%.2f", pos.x);

				if (ImGui::InputText("x ###px", Pos_x_c, sizeof(Pos_x_c))) {

					pos.x = atof(Pos_x_c);

				}



				char Pos_y_c[16] = {};

				sprintf_s(Pos_y_c, "%.2f", pos.y);

				if (ImGui::InputText("y ###py", Pos_y_c, sizeof(Pos_y_c))) {

					pos.y = atof(Pos_y_c);

				}


				char Pos_z_c[16] = {};

				sprintf_s(Pos_z_c, "%.2f", pos.z);

				if (ImGui::InputText("z ###pz", Pos_z_c, sizeof(Pos_z_c))) {

					float temp = atof(Pos_z_c);

					pos.z = temp;
				}

				transform->SetPosition(pos);
			}


			if (ImGui::CollapsingHeader("Rotation:")) {


				math::float3 e_rot = transform->GetRotation();

				char Rot_x_c[16] = {};

				sprintf_s(Rot_x_c, "%.2f", e_rot.x);

				if (ImGui::InputText("x ###rx", Rot_x_c, sizeof(Rot_x_c))) {

					e_rot.x = atof(Rot_x_c);

				}



				char Rot_y_c[16] = {};

				sprintf_s(Rot_y_c, "%.2f", e_rot.y);

				if (ImGui::InputText("y ###ry", Rot_y_c, sizeof(Rot_y_c))) {

					e_rot.y = atof(Rot_y_c);

				}


				char Rot_z_c[16] = {};

				sprintf_s(Rot_z_c, "%.2f", e_rot.z);

				if (ImGui::InputText("z ###rz", Rot_z_c, sizeof(Rot_z_c))) {

					e_rot.z = atof(Rot_z_c);

				}

				transform->SetRotation(e_rot);

			}

			if (ImGui::CollapsingHeader("Scale:")) {

				math::float3 scale = transform->GetScale();

				char Sc_x_c[16] = {};

				sprintf_s(Sc_x_c, "%.2f", scale.x);

				if (ImGui::InputText("x ###sx", Sc_x_c, sizeof(Sc_x_c))) {

					scale.x = atof(Sc_x_c);

				}



				char Sc_y_c[16] = {};

				sprintf_s(Sc_y_c, "%.2f", scale.y);

				if (ImGui::InputText("y ###sy", Sc_y_c, sizeof(Sc_y_c))) {

					scale.y = atof(Sc_y_c);
				}


				char Sc_z_c[16] = {};

				sprintf_s(Sc_z_c, "%.2f", scale.z);

				if (ImGui::InputText("z ###sz", Sc_z_c, sizeof(Sc_z_c))) {

					scale.z = atof(Sc_z_c);

				}
			}

		}
	}

	if (mesh != nullptr) {
		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Text("Indices: %i", mesh->GetMesh()->num_indices);

			ImGui::Text("Vertices: %i", mesh->GetMesh()->num_vertices);

			ImGui::Text("Uv's: %i", mesh->GetMesh()->num_uv);

			ImGui::Text("Triangles: %i", mesh->GetMesh()->num_vertices / 3);
		}
	}

	if (tex != nullptr) {
		if (ImGui::CollapsingHeader("Material Material", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::Checkbox("Checker Material", &material_cheker);
			ImGui::Checkbox("No Texture", &no_texture);
			if (material_cheker)
			{
				if (checker_id = 0) {
					checker_id = App->renderer3D->CreateCheckers();
				}
				ImGui::Image((ImTextureID)(checker_id), ImVec2(250, 250));
			}
			else
			{
				ImGui::Text("Width: %i", tex->GetTextureWidth());
				ImGui::SameLine();
				ImGui::Text("Height: %i", tex->GetTextureHeight());
				ImGui::Image((ImTextureID)(tex->GetTextureID()), ImVec2(250, 250));
			}
		}
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