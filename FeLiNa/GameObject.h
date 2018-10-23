#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_

#include "Globals.h"
#include <vector>

class Component;
enum ComponentType;

class GameObject
{
public:

	GameObject();
	GameObject(GameObject* parent);
	GameObject(char* name);
	GameObject(char* name, GameObject* parent , bool active = true);

	~GameObject();
	
	//bool Update(float dt);
	//bool PostUpdate(float dt);
	bool CleanUp();

	void SetName(char*name);
	char* GetName() const;

	void SetActive(bool* active);
	bool GetActive() const;

	void AddChildren(GameObject* child);
	GameObject* GetChild(uint position);

	void SetParent(GameObject* parent);
	GameObject* GetParent()const;

	void SetComponent(Component* component);
	void SetComponent(ComponentType type);
	void SetComponent(GameObject* parent, ComponentType type);
	
	bool DeleteComponent(Component* component);
	bool DeleteComponent(ComponentType type);

private:
	
	char* name = "No Name";
	bool active = true;
	std::vector<GameObject*> childrens;
	GameObject* parent = nullptr;
	std::vector<Component*> components;
};


#endif 
