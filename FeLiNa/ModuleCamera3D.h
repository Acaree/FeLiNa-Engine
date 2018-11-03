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
	void CheckObjectActive(GameObject* go);
	
	void PickObjectSelected(std::vector<GameObject*> &candidates ,math::LineSegment ray);

public:
	ComponentCamera* camera_editor = nullptr;
	GameObject* main_camera = nullptr;
	ComponentCamera* game_camera = nullptr;

private:
	
	ComponentTransform* transform_camera = nullptr;

	//Mouse picking test
	math::LineSegment picking;
	std::vector<GameObject*> posible_go_intersections;

	
};

#endif