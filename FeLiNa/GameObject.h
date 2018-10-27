#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_

#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"
#include <vector>

class Component;
enum ComponentType;
struct Mesh;

class GameObject
{
public:

	GameObject(GameObject* parent);

	~GameObject();
	
	void Update(float dt);
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

	void SetComponent(Component* parent);

	bool DeleteComponent(Component* component);
	bool DeleteComponent(ComponentType type);

	std::vector<Component*> components;

	uint GetNumChildren() const;

	void ShowObjectHierarchy();
	void ShowObjectInspector();

	void SetSelected(bool selected);
	bool IsSelected() const;
	void ToggleSelected();

	void AddBoundingBox(const Mesh* mesh);
	void DrawBoundingBox();

private:
	
	char* name = "No Name";
	bool active = true;
	std::vector<GameObject*> childrens;
	GameObject* parent = nullptr;
	bool selected = false;

	AABB* bounding_box = nullptr;
};


#endif 
