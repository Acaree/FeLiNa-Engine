#include "ModuleCamera3D.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";

	CalculateViewMatrix();

	X = math::float3(1.0F, 0.0F, 0.0F);
	Y = math::float3(0.0F, 1.0F, 0.0F);
	Z = math::float3(0.0F, 0.0F, 1.0F);

	Position = math::float3(0.0F, 0.0F, 5.0F);
	Reference = math::float3(0.0F, 0.0F, 0.0F);



	
}

ModuleCamera3D::~ModuleCamera3D()
{
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



	return ret;
}


// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");


	return true;
}

update_status ModuleCamera3D::PreUpdate(float dt)
{

	
	for (uint i = 0; i < App->scene->root_object->GetNumChildren(); ++i)
	{
		CheckObjectActive(App->scene->root_object->GetChild(i));
	}



	return UPDATE_CONTINUE;
}


// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	math::float3 newPos(0,0,0);
	float speed = 3.0F * dt;

	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0F * dt;

	if(App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		newPos.y += speed;
	
	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT)
		newPos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_T) == KEY_REPEAT)
		newPos.y -= speed;

	/*if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		// TODO-> Focus*/

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			newPos += X * speed;


		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.01F;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			math::float4x4 matrixX = math::float4x4::RotateAxisAngle(math::float3(0, 1, 0), DeltaX);
			math::float4 resultX = matrixX * math::float4(X.x, X.y, X.z, 1);
			math::float3 vectorX = math::float3(resultX.x, resultX.y, resultX.z);

			resultX = matrixX * math::float4(Y.x, Y.y, Y.z, 1);
			math::float3 vectorY = math::float3(resultX.x, resultX.y, resultX.z);

			resultX = matrixX * math::float4(Z.x, Z.y, Z.z, 1);
			math::float3 vectorZ = math::float3(resultX.x, resultX.y, resultX.z);

			X = vectorX;
			Y = vectorY;
			Z = vectorZ;

		}
		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			math::float4x4 matrixY = math::float4x4::RotateAxisAngle(X, DeltaY);
			math::float4 resultY = matrixY * math::float4(Y.x, Y.y, Y.z, 1);
			math::float3 vectorY = math::float3(resultY.x, resultY.y, resultY.z);

			matrixY =math::float4x4::RotateAxisAngle(X, DeltaY);
			resultY = matrixY * math::float4(Z.x, Z.y, Z.z, 1);
			math::float3 vectorZ = math::float3(resultY.x, resultY.y, resultY.z);

			Y = vectorY;

			Z = vectorZ;

			if (Y.y < 0.0F)
			{
				Z = math::float3(0.0F, Z.y > 0.0F ? 1.0F : -1.0F, 0.0F);
				Y = math::Cross(Z, X);
			}
		}

	}
	
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		if (App->input->GetMouseXMotion() > 0)
			newPos -= X * (App->input->GetMouseXMotion() * dt) * speed;
		
		if (App->input->GetMouseXMotion() < 0)
			newPos -= X * (App->input->GetMouseXMotion() * dt) * speed;
		
		if (App->input->GetMouseYMotion() > 0)
			newPos += Y * (App->input->GetMouseYMotion() * dt) * speed;
		
		if (App->input->GetMouseYMotion() < 0)
			newPos += Y * (App->input->GetMouseYMotion() * dt) * speed;
		
	}

	if (App->input->GetMouseZ() == 1)
		newPos -= Z *10* dt;
	
	if (App->input->GetMouseZ() == -1)
		newPos += Z *10* dt;
	
	Position += newPos;
	Reference += newPos;

	if(App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.01F;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			math::float3x3 rotationMatrix = math::float3x3::RotateY(DeltaX);
			X = rotationMatrix * X;
			Y = rotationMatrix * Y;
			Z = rotationMatrix * Z;
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			math::float3x3 rotationMatrix = math::float3x3::RotateAxisAngle(X, DeltaY);
			Y = rotationMatrix * Y;
			Z = rotationMatrix * Z;

			if (Y.y < 0.0F)
			{
				Z = math::float3(0.0F, Z.y > 0.0F ? 1.0F : -1.0F, 0.0F);
				Y = math::Cross(Z, X);
			}
		}

		Position = Reference + Z * Position.Length();
	}

	

	CalculateViewMatrix();

	dummy_frustum->DebugDraw();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const math::float3 &Position, const math::float3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = Position-Reference;
	Z.Normalize();
	X = math::float3(0.0F, 1.0F, 0.0F).Cross(Z);
	X.Normalize();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05F;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const math::float3 &Spot)
{
	Reference = Spot;

	Z = Position - Reference;
	Z.Normalize();
	X = math::float3(0.0F, 1.0F, 0.0F).Cross(Z);
	X.Normalize();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const math::float3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix() const
{
	return (float*)&ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = math::float4x4(X.x, Y.x, Z.x, 0.0F, X.y, Y.y, Z.y, 0.0F, X.z, Y.z, Z.z, 0.0F, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0F);

	math::float4x4 inverse = ViewMatrix;
	inverse.Inverse();
	ViewMatrixInverse = inverse;
}


void ModuleCamera3D::MoveCamera(math::float3 newPos, float speed)
{/*
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
			newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			newPos += X * speed;


		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.01F;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			math::float4x4 matrixX = math::math::float4x4::RotateAxisAngle(math::float3(0, 1, 0), DeltaX);
			math::float4 resultX = matrixX * math::float4(X.x, X.y, X.z, 1);
			math::float3 vectorX = math::float3(resultX.x, resultX.y, resultX.z);

			resultX = matrixX * math::float4(Y.x, Y.y, Y.z, 1);
			math::float3 vectorY = math::float3(resultX.x, resultX.y, resultX.z);

			resultX = matrixX * math::float4(Z.x, Z.y, Z.z, 1);
			math::float3 vectorZ = math::float3(resultX.x, resultX.y, resultX.z);

			X = vectorX;
			Y = vectorY;
			Z = vectorZ;

		}
		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			math::float4x4 matrixY = math::math::float4x4::RotateAxisAngle(X, DeltaY);
			math::float4 resultY = matrixY * math::float4(Y.x, Y.y, Y.z, 1);
			math::float3 vectorY = math::float3(resultY.x, resultY.y, resultY.z);

			matrixY = math::math::float4x4::RotateAxisAngle(X, DeltaY);
			resultY = matrixY * math::float4(Z.x, Z.y, Z.z, 1);
			math::float3 vectorZ = math::float3(resultY.x, resultY.y, resultY.z);

			Y = vectorY;

			Z = vectorZ;

			if (Y.y < 0.0F)
			{
				Z = math::float3(0.0F, Z.y > 0.0F ? 1.0F : -1.0F, 0.0F);
				Y = math::Cross(Z, X);
			}
		}

	}
	*/
}

void ModuleCamera3D::CheckObjectActive(GameObject* go)
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