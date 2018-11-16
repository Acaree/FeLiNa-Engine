#ifndef _GAME_OBJECT_
#define _GAME_OBJECT_
#include "Globals.h"
#include "Parson/parson.h"
#include "MathGeoLib/MathGeoLib.h"
#include <vector>

class Component;
enum ComponentType;
struct Mesh;
class Resource;
class ComponentTransform;
class ComponentTexture;
class ComponentMesh;
class ComponentCamera;

class GameObject
{
public:

	GameObject(GameObject* parent);

	~GameObject();
	
	void Update(float dt);
	bool CleanUp();

	void CleanData();
	void SetName(const char*name);
	char* GetName() const;

	void SetActive(bool* active);
	bool GetActive() const;

	void AddChildren(GameObject* child);
	GameObject* GetChild(uint position);
	void DeleteChildren(GameObject* child);


	void SetParent(GameObject* parent);
	GameObject* GetParent()const;

	void SetComponent(Component* parent);
	Component* GetComponent(ComponentType type);
	Component* AddComponent(ComponentType type);

	//void CloneComponents(Component* component);
	bool DeleteComponent(Component* component);
	void DeleteAllComponents();
	bool DeleteComponent(ComponentType type);

	std::vector<Component*> components;

	uint GetNumChildren() const;

	void ShowObjectHierarchy();
	void ShowObjectInspector();

	void SetSelected(bool selected);
	bool IsSelected() const;
	void ToggleSelected();

	//void AddBoundingBox(const Mesh* mesh);
	math::AABB GetAABB() const;
	void DrawBoundingBox();
	void RecalculateBoundingBox();

	void SetActive(bool active);
	bool IsActive() const;

	bool static_object = false;
	math::AABB bounding_box;

	void OnSave(JSON_Object* obj);
	void OnLoad(JSON_Object* obj);

	void ShowGameObjectOptions();

	uint uid = 0;

	GameObject* SearchParentForUID(uint parent_uid);
	
	void SetInvalidateResource(const Resource* resource);

public:

	std::vector<GameObject*> childrens;
	// I think this is best 
	ComponentTransform* transform = nullptr;
	ComponentTexture* material = nullptr;
	ComponentMesh* mesh = nullptr;
	ComponentCamera* camera = nullptr;
	bool to_delete = false;

private:
	
	char name[DEFAULT_BUF_SIZE];
	

	GameObject* parent = nullptr;
	bool selected = false;
	bool active = true;


};


#endif 
