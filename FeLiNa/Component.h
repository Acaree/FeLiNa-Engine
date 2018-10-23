#ifndef _COMPONENT_
#define _COMPONENT_

class GameObject;

enum ComponentType
{
	ComponentTransform,
	ComponenMesh,
	ComponentMaterial,
	ComponentDefault
};



class Component
{
public:

	Component();
	Component(ComponentType type = ComponentDefault);
	Component(GameObject* parent,ComponentType type = ComponentDefault);
	Component(GameObject* parent, ComponentType type = ComponentDefault, bool active = true);

	~Component();

	void SetComponentType(ComponentType type = ComponentDefault);
	ComponentType GetComponentType() const;

	void SetActive(bool active = true);
	bool GetActive() const;

	void SetParent(GameObject* parent = nullptr);
	GameObject* GetParent() const;

private:
	ComponentType type = ComponentDefault;
	bool active = true;
	GameObject* parent = nullptr;

};



#endif
