#ifndef _MODULE_SCENE_
#define _MODULE_SCENE_

#include "Module.h"
#include "mPlane.h"
#include <list>
class GameObject;
enum ComponentType;
class QuadTree;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void ShowHierarchy();
	void ShowInspector();	

	void SetSelectedGameObject(GameObject* go);
	GameObject* GetSelectedGameObject() const;

private:

	mPlane* grid_plane = nullptr;
	bool hierarchy_open = true;
	bool inspector_open = true;
	GameObject* selected = nullptr;

	uint checker_id = 0;
	bool material_cheker = false;
	bool no_texture = false;

public:
	std::list<GameObject*> static_go;
	GameObject* root_object = nullptr;
	QuadTree * quadtree = nullptr;
};

#endif