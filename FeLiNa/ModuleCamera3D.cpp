#include "ModuleCamera3D.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "MathGeoLib/MathGeoLib.h"
#include "ModuleWindow.h"
#include "Quadtree.h"
#include "ModuleResourceManager.h"
#include "ResourceMesh.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include "ImGuizmo/ImGuizmo.h"
#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";
}

ModuleCamera3D::~ModuleCamera3D()
{
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;


#ifndef  GAME_MODE
	//Create and Set Editor camera a initial pos
	camera_editor = new ComponentCamera(nullptr);
	camera_editor->frustum.Translate(math::float3(5, 10, 5));
	current_camera = camera_editor;

#endif // ! GAME_MODE

	LookAt(math::float3::zero);

	return ret;
}


// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	RELEASE(camera_editor);

	return true;
}


// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	math::float3 new_position = math::float3::zero;
	float speed = 3.0F * dt;

	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0F * dt;

	if(App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		new_position.y += speed;
	
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
		new_position.y -= speed;


#ifndef GAME_MODE
	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN )
	{
		if (!ImGui::IsMouseHoveringAnyWindow() && !ImGuizmo::IsOver())
		{
			//Taking screen values.
			float width = (float)App->window->width;
			float height = (float)App->window->height;

			//Take mouse position.
			int mouse_x = App->input->GetMouseX();
			int mouse_y = App->input->GetMouseY();

			//if stack overflow say :/ Theory: set the 0,0 in middle of screen.
			float normalized_x = -(1.0f - (float(mouse_x) * 2.0f) / width);
			float normalized_y = 1.0f - (float(mouse_y) * 2.0f) / height;

			math::LineSegment picking = current_camera->frustum.UnProjectLineSegment(normalized_x, normalized_y);
			PickObjectSelected(posible_go_intersections, picking);
		}
	}
#endif

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT )
	{

		//Keyboard Movement with Frustum--------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			new_position += current_camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			new_position -= current_camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			new_position -= current_camera->frustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			new_position += current_camera->frustum.WorldRight() * speed;

		
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		current_camera->frustum.Translate(new_position);

		if (dx != 0.0f)
		{
			math::Quat quat = math::Quat::RotateY(dx*dt*0.3);
			current_camera->frustum.front = quat.Mul(current_camera->frustum.front).Normalized();
			current_camera->frustum.up = quat.Mul(current_camera->frustum.up).Normalized();
		}

		if (dy != 0.0f)
		{
			math::Quat quat = math::Quat::RotateAxisAngle(current_camera->frustum.WorldRight(), dy * dt *0.3);
			math::float3 up = quat.Mul(current_camera->frustum.up).Normalized();
			current_camera->frustum.up = up;
			current_camera->frustum.front = quat.Mul(current_camera->frustum.front).Normalized();
		}

	}
	

	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const math::float3 &position)
{
	math::float3 direction = position - current_camera->frustum.pos;
	math::float3x3 matrix = math::float3x3::LookAt(current_camera->frustum.front, direction.Normalized(), current_camera->frustum.up, math::float3(0, 1, 0));

	current_camera->frustum.front = matrix.MulDir(current_camera->frustum.front).Normalized();
	current_camera->frustum.up = matrix.MulDir(current_camera->frustum.up).Normalized();
}


void ModuleCamera3D::PickObjectSelected(std::vector<GameObject*> &candidates, math::LineSegment ray)
{
	candidates.clear(); //Else all time add elements

	App->scene->quadtree->CollectIntersections(candidates, ray); 

	for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
	{
		PosibleObjectsPicked( candidates , App->scene->root_object->GetChild(i));
	}

	float min_distance = FLOAT_INF;
	float hit_distance = 0.0F;

	GameObject* selected_object = nullptr;

	for (uint i = 0; i < candidates.size(); ++i)
	{
		math::Triangle triangle;

		//To calculate the local space of ray in object
		math::LineSegment ray_local(ray);

		ComponentTransform* component_transform = candidates[i]->transform;

		if (component_transform != nullptr) //Allways is != but...
		{
			//Transform to local 
			ray_local.Transform(component_transform->GetTransformMatrix().Inverted());

			ComponentMesh* component_mesh = candidates[i]->mesh;


			if (component_mesh != nullptr)
			{
				//Set the object mesh
				ResourceMesh* mesh = (ResourceMesh*)App->resource_manager->Get(component_mesh->GetUID());

				int j = 0;

				while (j < mesh->num_indices)
				{
					math::float3 x = {mesh->vertices[mesh->indices[j]*3],mesh->vertices[mesh->indices[j]*3+1] ,mesh->vertices[mesh->indices[j]*3+2] };
					j++;
					math::float3 y = { mesh->vertices[mesh->indices[j]*3],mesh->vertices[mesh->indices[j]*3+1] ,mesh->vertices[mesh->indices[j]*3+2] };
					j++;
					math::float3 z = { mesh->vertices[mesh->indices[j]*3],mesh->vertices[mesh->indices[j]*3+1] ,mesh->vertices[mesh->indices[j]*3+2] };
					j++;

					triangle = { x,y,z };
					math::float3 hit_point;

					if (ray_local.Intersects(triangle, &hit_distance, &hit_point))
					{
						if (hit_distance < min_distance)
						{
							selected_object = candidates[i];
							min_distance = hit_distance;
						}
					}

				}

			}

		}


	}

	if (selected_object != nullptr)
	{
		GameObject* go =App->scene->GetSelectedGameObject();
		if (go != nullptr)
		{
			go->SetSelected(false);
		}
		selected_object->SetSelected(true);
		App->scene->SetSelectedGameObject(selected_object);
	}

}

void ModuleCamera3D::PosibleObjectsPicked(std::vector<GameObject*> &posible_candidate, GameObject* candidates)
{

	if (current_camera->ContainsAaBox(candidates->GetAABB()))
	{
		posible_candidate.push_back(candidates);
	}

	for (uint i = 0; i < candidates->GetNumChildren(); ++i)
	{
		PosibleObjectsPicked(posible_candidate, candidates->GetChild(i));
	}

}