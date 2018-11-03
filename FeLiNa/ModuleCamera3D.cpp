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
#include "ModuleRenderer3D.h"

#include "mmgr/mmgr.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";
}

ModuleCamera3D::~ModuleCamera3D()
{
	//At this point the components are deleted.

	/*RELEASE(camera_editor);
	RELEASE(dummy_frustum);
	RELEASE(main_camera);
	RELEASE(transform_camera);

	for (uint i = 0; i < posible_go_intersections.size(); ++i)
	{
		posible_go_intersections[i]->CleanUp();
		RELEASE(posible_go_intersections[i]);
	}
	posible_go_intersections.clear();*/

}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	main_camera = new GameObject(nullptr);
	main_camera->SetName("Main Camera");

	ComponentTransform* transform =(ComponentTransform*) main_camera->AddComponent(Component_Transform);
	//transform_camera = new ComponentTransform(main_camera);
	game_camera = new ComponentCamera(main_camera);

	/*main_camera->SetComponent(transform_camera);
	main_camera->SetComponent(game_camera);*/
	
	ComponentCamera* camera = (ComponentCamera*)main_camera->AddComponent(Component_Camera);
	camera = game_camera;

	//Create and Set Edito camera a initial pos
	camera_editor = new ComponentCamera(nullptr);
	camera_editor->frustum.Translate(math::float3(5,10,5));
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

update_status ModuleCamera3D::PreUpdate(float dt)
{

	//To other module
	//TO REVISE
	for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
	{
		CheckObjectActive(App->scene->root_object->GetChild(i));
	}

	return UPDATE_CONTINUE;
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

	/*if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		// TODO-> Focus*/


	if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		//Taking screen values.
		float width = (float)App->window->screen_surface->w;
		float height = (float)App->window->screen_surface->h;

		//Take mouse position.
		int mouse_x = App->input->GetMouseX();
		int mouse_y = App->input->GetMouseY();

		//if stack overflow say :/ Theory: set the 0,0 in middle of screen.
		float normalized_x = -(1.0f - (float(mouse_x) * 2.0f) / width);
		float normalized_y = 1.0f - (float(mouse_y) * 2.0f) / height;

		math::LineSegment picking = camera_editor->frustum.UnProjectLineSegment(normalized_x, normalized_y);
		PickObjectSelected(posible_go_intersections, picking);
	}


	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{

		//Keyboard Movement with Frustum--------------------------------------
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			new_position += camera_editor->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			new_position -= camera_editor->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			new_position -= camera_editor->frustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			new_position += camera_editor->frustum.WorldRight() * speed;

		camera_editor->frustum.Translate(new_position);
		

		//Mouse movement with Frustum---------------------------------------
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion() ;

		if (dx != 0.0f)
		{
			math::Quat quat = math::Quat::RotateY(dx*dt*0.3); 
			camera_editor->frustum.front = quat.Mul(camera_editor->frustum.front).Normalized();
			camera_editor->frustum.up = quat.Mul(camera_editor->frustum.up).Normalized();
		}

		if (dy != 0.0f)
		{
			math::Quat quat = math::Quat::RotateAxisAngle(camera_editor->frustum.WorldRight(), dy * dt *0.3);
			math::float3 up = quat.Mul(camera_editor->frustum.up).Normalized();
			camera_editor->frustum.up = up;
			camera_editor->frustum.front = quat.Mul(camera_editor->frustum.front).Normalized();
		}

	}
	
	//TO SCENE
	//dummy_frustum->DebugDraw();

	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const math::float3 &position)
{
	math::float3 direction = position - camera_editor->frustum.pos;
	math::float3x3 matrix = math::float3x3::LookAt(camera_editor->frustum.front, direction.Normalized(), camera_editor->frustum.up, math::float3(0, 1, 0));

	camera_editor->frustum.front = matrix.MulDir(camera_editor->frustum.front).Normalized();
	camera_editor->frustum.up = matrix.MulDir(camera_editor->frustum.up).Normalized();
}

//TO OTHER
void ModuleCamera3D::CheckObjectActive(GameObject* go)
{
	if (!go->static_object)
	{
		Culling cull = main_camera->camera->ContainsAaBox(go->GetAABB());

		if (cull == Culling::CULL_OUT)
			go->SetActive(false);
		else
			go->SetActive(true);

		for (uint i = 0; i < go->GetNumChildren(); ++i)
		{
			CheckObjectActive(go->GetChild(i));
		}
	}
}

void ModuleCamera3D::PickObjectSelected(std::vector<GameObject*> &candidates, math::LineSegment ray)
{
	candidates.clear(); //Else all time add elements

	App->scene->quadtree->CollectIntersections(candidates, ray); // not order

	float min_distance = FLOAT_INF;
	float hit_distance = 0.0F;

	Mesh* mesh = nullptr;

	GameObject* selected_object = nullptr;

	for (uint i = 0; i < candidates.size(); ++i)
	{
		math::Triangle triangle;

		//To calculate the local space of ray in object
		math::LineSegment ray_local(ray);

		ComponentTransform* component_transform = (ComponentTransform*)candidates[i]->GetComponent(Component_Transform);

		if (component_transform != nullptr) //Allways is != but...
		{
			//Transform to local 
			ray_local.Transform(component_transform->GetTransformMatrix().Inverted());

			ComponentMesh* component_mesh = (ComponentMesh*)candidates[i]->GetComponent(Component_Mesh);
			//Set the object mesh
			mesh = component_mesh->GetMesh();

			if (component_mesh != nullptr)
			{
				int j = 0;

				while (j < mesh->num_indices)
				{
					math::float3 x = {mesh->vertices[mesh->indices[j++]],mesh->vertices[mesh->indices[j++]] ,mesh->vertices[mesh->indices[j++]] };
					math::float3 y = { mesh->vertices[mesh->indices[j++]],mesh->vertices[mesh->indices[j++]] ,mesh->vertices[mesh->indices[j++]] };
					math::float3 z = { mesh->vertices[mesh->indices[j++]],mesh->vertices[mesh->indices[j++]] ,mesh->vertices[mesh->indices[j++]] };

					triangle = { x,y,z };
					math::float3 hit_point;

					if (ray_local.Intersects(triangle, &hit_distance, &hit_point))
					{
						selected_object = candidates[i];
					}

				}

			}

		}


	}

	if (selected_object != nullptr)
	{
		App->scene->GetSelectedGameObject()->SetSelected(false);
		selected_object->SetSelected(true);
		App->scene->SetSelectedGameObject(selected_object);
	}

}