#pragma once

#include "Module.h"
#include "Primitive.h"

#include "MathGeoLib/MathGeoLib.h"

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	float vertexs[36];
	uint indices[8] = {2,1,0,0,1,2};
	uint my_id = 0;
	uint my_indices = 0;

	mPlane* grid_plane;

	//Sito Geometry
	
	float3 origin = { 0.0F, 0.0F, 0.0F };

	/// Sphere: 2 circles
	uint sphere_id = 0;
	uint sphere_indices_id = 0;
	float radius_circle = 1.0F;
	float circle_angle = 30.0F;
	float3 vertex_circle[27]; // 1 center + 12 angles
	uint indices_circle[72]; // 1 center + 12 angles

};
