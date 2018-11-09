#ifndef _MODULE_CAMERA_3D
#define _MODULE_CAMERA_3D

#include "Globals.h"
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
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void LookAt(const math::float3 &Spot);
	
	void PickObjectSelected(std::vector<GameObject*> &candidates ,math::LineSegment ray);
	void PosibleObjectsPicked(std::vector<GameObject*> &posible_candidates , GameObject* candidate);

public:

	/* Camera editor: see in editor mode
		Game Camera: see in game mode
		Main Camera: GameObject for print and move in editor and game mode
		Current Camera: all logic.
	
	*/

	ComponentCamera* camera_editor = nullptr;
	GameObject* main_camera = nullptr;
	ComponentCamera* game_camera = nullptr;

	ComponentCamera* current_camera = nullptr;

private:
	
	ComponentTransform* transform_camera = nullptr;

	//Mouse picking test
	math::LineSegment picking;
	std::vector<GameObject*> posible_go_intersections;

	
};

#endif