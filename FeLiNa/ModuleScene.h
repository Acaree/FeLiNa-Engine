#ifndef _MODULE_SCENE_
#define _MODULE_SCENE_

#include "Module.h"
#include "mPlane.h"

class GameObject;
enum ComponentType;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void CreateGameObject(GameObject* object);

	bool DeleteGameObject(GameObject* object);
	bool DeleteGameObject(char* name);

	void ShowHierarchy();
	void ShowInspector();

	std::vector<GameObject*> game_objects;

	GameObject* root_object = nullptr;

	void SetSelectedGameObject(GameObject* go);
	GameObject* GetSelectedGameObject() const;
private:

	mPlane* grid_plane = nullptr;
	bool hierarchy_open = true;
	bool inspector_open = true;
	GameObject* selected = nullptr;

};

#endif