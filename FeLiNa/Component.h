#ifndef _COMPONENT_
#define _COMPONENT_

#include "Globals.h"
#include "Parson/parson.h"
class GameObject;


enum ComponentType
{
	Component_Transform,
	Component_Mesh,
	Component_Material,
	Component_Camera,
	Component_Speed,
	Component_Script,
	Component_Default

};

class Component
{
public:

	Component(GameObject* parent);
	~Component();

	virtual void Update(float dt);
	virtual void CleanUp();

	void SetComponentType(ComponentType type = Component_Default);
	ComponentType GetComponentType() const;

	void SetActive(bool active = true);
	bool GetActive() const;

	void SetParent(GameObject* parent = nullptr);
	GameObject* GetParent() const;

	virtual void DrawInspector();

	virtual void OnSave(JSON_Object* obj);
	virtual void OnLoad(JSON_Object* obj);

protected:

	bool active = true;
	GameObject* parent = nullptr;
	uint uid = 0;
	ComponentType type = Component_Default;
};



#endif
