#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_

#include "Globals.h"
#include <vector>


class GameObject
{
public:

	GameObject();
	GameObject(GameObject* parent);
	GameObject(char* name = "No Name");
	GameObject(char* name, GameObject* parent = nullptr, bool active = true);

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



private:
	
	char* name = "No Name";
	bool active = true;
	std::vector<GameObject*> childrens;
	GameObject* parent = nullptr;
};


#endif 
