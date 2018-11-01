#include "ModuleCamera3D.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "MathGeoLib/MathGeoLib.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";
}

ModuleCamera3D::~ModuleCamera3D()
{
	RELEASE(camera_editor);
	
	//ALL DUMMY TO SCENE.
	delete dummy_frustum;
	dummy_frustum = nullptr;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	main_camera = new GameObject(nullptr);
	main_camera->SetName("Main Camera");

	transform_camera = new ComponentTransform(main_camera);
	dummy_frustum = new ComponentCamera(main_camera);

	main_camera->SetComponent(transform_camera);
	main_camera->SetComponent(dummy_frustum);
	
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
	/*for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
	{
		CheckObjectActive(App->scene->root_object->GetChild(i));
	}*/

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
	dummy_frustum->DebugDraw();

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
		Culling cull = dummy_frustum->ContainsAaBox(go->GetAABB());

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