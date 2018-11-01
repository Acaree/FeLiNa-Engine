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


	Component* GetComponent(ComponentType type);

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
	math::AABB GetAABB() const;
	void DrawBoundingBox();
	void RecalculateBoundingBox();

	void SetActive(bool active);
	bool IsActive() const;

	bool static_object = false;
	math::AABB bounding_box;
private:
	
	char* name = "No Name";
	
	std::vector<GameObject*> childrens;
	GameObject* parent = nullptr;
	bool selected = false;

	uint uid = 0;

	bool active = false;

};


#endif 
