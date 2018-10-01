#pragma once

#include "Module.h"
#include "Primitive.h"


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
	uint indices[8] = {0,1,2};
	uint my_id = 0;
	uint my_indices = 0;

	mPlane* grid_plane;


};
