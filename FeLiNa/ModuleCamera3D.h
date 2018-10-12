#ifndef _MODULE_CAMERA_3D
#define _MODULE_CAMERA_3D

#include "Module.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);
	float* GetViewMatrix();

	void CreateFocusObject(float3 min,float3 max);
	void FocusToCenterObject();

private:

	void CalculateViewMatrix();

public:
	
	float3 X, Y, Z, Position, Reference;

	AABB* focus_box = nullptr;

private:

	float4x4 ViewMatrix, ViewMatrixInverse;

};

#endif