#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib/MathGeoLib.h"
#include "MathGeoLib/Math/TransformOps.h"
ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";

	CalculateViewMatrix();

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 0.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG_GLOBAL("Setting up the camera");
	bool ret = true;

	return ret;
}


update_status ModuleCamera3D::PreUpdate(float dt) {

	update_status update_return = UPDATE_CONTINUE;
	module_timer.Start();

	return update_return;
}

update_status ModuleCamera3D::PostUpdate(float dt) {


	update_status update_return = UPDATE_CONTINUE;
	last_update_ms = module_timer.ReadMs();
	module_timer.Start();

	return update_return;

}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG_GLOBAL("Cleaning camera");


	if (focus_box != nullptr)
		delete focus_box;

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	float3 newPos(0,0,0);
	float speed = 3.0f * dt;
	if(App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * dt;

	if(App->input->GetKey(SDL_SCANCODE_R) == KEY_REPEAT) newPos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) FocusToCenterObject();//newPos.y -= speed;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) newPos -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) newPos += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPos -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPos += X * speed;

	}
	//Pan we need this?
	if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT)
	{
		if (App->input->GetMouseXMotion() > 0)
		{
			newPos -= X * (App->input->GetMouseXMotion() * dt) * speed;
		}
		if (App->input->GetMouseXMotion() < 0)
		{
			newPos -= X * (App->input->GetMouseXMotion() * dt) * speed;
		}
		if (App->input->GetMouseYMotion() > 0)
		{
			newPos += Y * (App->input->GetMouseYMotion() * dt) * speed;
		}
		if (App->input->GetMouseYMotion() < 0)
		{
			newPos += Y * (App->input->GetMouseYMotion() * dt) * speed;
		}
	}

	//TO REVISION ZOOM
	if (App->input->GetMouseZ() == 1)
	{
		newPos -= Z *10* dt;
	}
	if (App->input->GetMouseZ() == -1)
	{
		newPos += Z *10* dt;
	}

	Position += newPos;
	Reference += newPos;

	// Mouse motion ----------------

	if(App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.01f;

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

			if (Y.y < 0.0f)
			{
				Z = math::float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = math::Cross(Z, X);
			}
		}

		Position = Reference + Z * Position.Length();
	}

	//TO TEST ROTATE NOT FINISH
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_IDLE && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		
			int dx = -App->input->GetMouseXMotion();
			int dy = -App->input->GetMouseYMotion();

			float Sensitivity = 0.01f;
			
			if (dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				

				float4x4 matrixX = math::float4x4::RotateAxisAngle(float3(0, 1, 0), DeltaX);
				float4 resultX = matrixX * float4(X.x, X.y, X.z, 1);
				float3 vectorX = float3(resultX.x, resultX.y,resultX.z);

				resultX = matrixX * float4(Y.x,Y.y, Y.z, 1);
				float3 vectorY = float3(resultX.x, resultX.y, resultX.z);

				resultX = matrixX * float4(Z.x, Z.y, Z.z, 1);
				float3 vectorZ = float3(resultX.x, resultX.y, resultX.z);


				X = vectorX ;
				Y =  vectorY;
				Z = vectorZ ;
			

			}
			if (dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity;

				float4x4 matrixY = math::float4x4::RotateAxisAngle(X, DeltaY);
				float4 resultY= matrixY * float4(Y.x, Y.y, Y.z, 1);
				float3 vectorY = float3(resultY.x, resultY.y, resultY.z);

				matrixY = math::float4x4::RotateAxisAngle(X, DeltaY);
				resultY = matrixY * float4(Z.x, Z.y, Z.z, 1);
				float3 vectorZ = float3(resultY.x, resultY.y, resultY.z);

				Y = vectorY;
				
				Z = vectorZ;

				if (Y.y < 0.0f)
				{
					Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					Y = math::Cross(Z, X);
				}
			}
	}


	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = Position-Reference;
	Z.Normalize();
	X = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X.Normalize();
	Y = Z.Cross(X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	Reference = Spot;

	Z = Position - Reference;
	Z.Normalize();
	X = float3(0.0f, 1.0f, 0.0f).Cross(Z);
	X.Normalize();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return (float*)&ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);

	float4x4 inverse = ViewMatrix;
	inverse.Inverse();
	ViewMatrixInverse = inverse;
}

void ModuleCamera3D::CreateFocusObject(float3 min, float3 max)
{
	if (focus_box != nullptr)
	{
		focus_box->minPoint = min;
		focus_box->maxPoint = max;
	}
	else
	{
		focus_box = new AABB(min, max);
	}
}

void ModuleCamera3D::FocusToCenterObject()
{
	if (focus_box != nullptr)
	{

		Reference = focus_box->CenterPoint();
		Position = focus_box->CenterPoint() + focus_box->Size();
		LookAt(Reference);
	}
}