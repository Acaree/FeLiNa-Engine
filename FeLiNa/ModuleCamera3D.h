#ifndef _MODULE_CAMERA_3D
#define _MODULE_CAMERA_3D

#include "Module.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentCamera;
class GameObject;
class ComponentTransform;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const math::float3 &Position, const math::float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const math::float3 &Spot);
	void Move(const math::float3 &Movement);

	void MoveCamera(math::float3 newPos, float speed);
	
	
	float* GetViewMatrix() const;

private:

	void CalculateViewMatrix();

public:
	
	math::float3 X = { 0,0,0 };
	math::float3 Y = { 0,0,0 };
	math::float3 Z = { 0,0,0 };
	math::float3 Position = { 0,0,0 };
	math::float3 Reference = {0,0,0};

	GameObject* main_camera = nullptr;

private:

	math::float4x4 ViewMatrix, ViewMatrixInverse;

	
	ComponentCamera* dummy_frustum = nullptr;
	ComponentTransform* transform_camera = nullptr;
};

#endif