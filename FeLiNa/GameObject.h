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
class ComponentMaterial;
class ComponentMesh;
class ComponentCamera;
class ComponentScript;

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
	GameObject* GetChild(uint position) const;
	void DeleteChildren(GameObject* child);
	void ExtractChildrenFromList(GameObject* go);

	void SetParent(GameObject* parent);
	GameObject* GetParent()const;

	void SetComponent(Component* parent);
	Component* GetComponent(const ComponentType type);
	Component* AddComponent(const ComponentType type);

	bool DeleteComponent(Component* component);
	void DeleteAllComponents();
	bool DeleteComponent(ComponentType type);

	std::vector<Component*> components;

	uint GetNumChildren() const;

	void ShowObjectHierarchy();
	void ShowObjectInspector();

	void SetSelected(const bool selected);
	bool IsSelected() const;
	void ToggleSelected();

	math::AABB GetAABB() const;
	void DrawBoundingBox();
	void RecalculateBoundingBox();

	void SetActive(const bool active);
	bool IsActive() const;

	bool static_object = false;
	math::AABB bounding_box;

	void OnSave(JSON_Object* obj);
	void OnLoad(JSON_Object* obj);

	void ShowGameObjectOptions();

	uint uid = 0;

	GameObject* SearchParentForUID(const uint parent_uid);
	
	void SetInvalidateResource(const Resource* resource);

public:

	std::vector<GameObject*> childrens;

	ComponentTransform* transform = nullptr;
	ComponentMaterial* material = nullptr;
	ComponentMesh* mesh = nullptr;
	ComponentCamera* camera = nullptr;
	ComponentScript* script = nullptr;
	bool to_delete = false;

private:
	
	char name[DEFAULT_BUF_SIZE];
	
	GameObject* parent = nullptr;
	bool selected = false;
	bool active = true;

};


#endif 
