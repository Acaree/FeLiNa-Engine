#pragma once

#include "Module.h"
#include "Primitive.h"

#include "Cube_Arrays.h"
#include "Cube_ArraysandIndex.h"
#include "Cube_DirectMode.h"
#include "GLLine.h"
#include "Arrow.h"
#include "Frustrum.h"

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

	Frustrum* frust;
	mPlane* grid_plane;

	//Sito Geometry
	
	float3 origin = {0.0F, 1.0F, 0.0F };

	/// Sphere: 2 circles
	uint sphere_id = 0;
	uint sphere_indices_id = 0;
	float radius_circle = 1.0F;
	float circle_angle = 30.0F;
	float3 vertex_circle[27]; 
	uint indices_circle[72];

	///Cylindre
	float cylinder_angle = 30.F;
	uint cylinder_id = 0;
	uint cylinder_indices_id = 0;
	float3 vertex_cylinder[28];
	uint indices_cylinder[145];
	
	float radius_cylinder = 1.0F;
	float latitude = 0.5F;

	///Capsule
	float capsule_angle = 30.F;
	uint capsule_id = 0;
	uint capsule_indices_id = 0;
	float3 vertex_capsule[28];
	uint indices_capsule[145];

	float radius_capsule = 1.0F;
	float latitude_capsule = 1.0F;
};
