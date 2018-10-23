#ifndef _COMPONENT_
#define _COMPONENT_

class GameObject;

enum ComponentType
{
	ComponentDefault,
	ComponentTransform,
	ComponenMesh,
	ComponentMaterial

};

class Component
{
public:

	Component(GameObject* parent);

	void CleanUp();

	~Component();

	virtual void Update(float dt);
	virtual void Draw();

	void SetComponentType(ComponentType type = ComponentDefault);
	ComponentType GetComponentType() const;

	void SetActive(bool active = true);
	bool GetActive() const;

	void SetParent(GameObject* parent = nullptr);
	GameObject* GetParent() const;

protected:

	ComponentType type = ComponentDefault;
	bool active = true;
	GameObject* parent = nullptr;

};



#endif
