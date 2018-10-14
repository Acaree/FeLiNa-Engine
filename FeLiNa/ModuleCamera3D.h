#ifndef _MODULE_CAMERA_3D
#define _MODULE_CAMERA_3D

#include "Module.h"
#include "MathGeoLib/MathGeoLib.h"


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
	float* GetViewMatrix() const;

	void CreateFocusObject(float3 min,float3 max);
	void GetMinMaxAABB(float3 &min, float3 &max);
	void FocusToCenterObject();

private:

	void CalculateViewMatrix();

public:
	
	float3 X = { 0,0,0 };
	float3 Y = { 0,0,0 };
	float3 Z = { 0,0,0 };
	float3 Position = { 0,0,0 };
	float3 Reference = {0,0,0};

	AABB* focus_box = nullptr;

private:

	float4x4 ViewMatrix, ViewMatrixInverse;

};

#endif