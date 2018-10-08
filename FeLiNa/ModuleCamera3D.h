#pragma once
#include "Module.h"
#include "Globals.h"

#include "MathGeoLib/MathGeoLib.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
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

	// TO REVISION to center camera in object
	

};