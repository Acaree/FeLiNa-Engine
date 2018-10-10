#ifndef _MODULE_SCENE_
#define _MODULE_SCENE_

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

	mPlane* grid_plane;
	GLuint ImageName;

};

#endif