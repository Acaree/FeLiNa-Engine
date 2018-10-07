#pragma once

#include "Module.h"
#include "Primitive.h"

#include "Cube_Arrays.h"
#include "Cube_ArraysandIndex.h"
#include "Cube_DirectMode.h"
#include "GLLine.h"
#include "Arrow.h"
#include "Frustrum.h"
#include "Cylindre.h"
#include "GLSphere.h"
#include "GLCapsule.h"

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


};
