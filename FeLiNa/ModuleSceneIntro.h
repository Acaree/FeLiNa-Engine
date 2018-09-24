#pragma once

#include "Module.h"
#include "Primitive.h"
#include "SDL/include/SDL_opengl.h"
#include "Pcg/pcg_variants.h"




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


private:
	mPlane* grid_plane;


};
