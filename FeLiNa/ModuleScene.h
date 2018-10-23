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

	void CreateGameObject();
	void CreateGameObject(GameObject* object);
	void CreateGameObject(char* name);
	void CreateGameObject(char* name, GameObject* parent, bool active = true);

	bool DeleteGameObject(GameObject* object);
	bool DeleteGameObject(char* name);

private:

	mPlane* grid_plane = nullptr;
	std::vector<GameObject*> game_objects;
	
};

#endif