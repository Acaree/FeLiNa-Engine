#ifndef _COMPONENT_
#define _COMPONENT_

class GameObject;

enum ComponentType
{
	Component_Transform,
	Component_Mesh,
	Component_Material,
	Component_Default

};

class Component
{
public:

	Component(GameObject* parent);

	void CleanUp();

	~Component();

	virtual void Update(float dt);
	virtual void Draw();

	void SetComponentType(ComponentType type = Component_Default);
	ComponentType GetComponentType() const;

	void SetActive(bool active = true);
	bool GetActive() const;

	ComponentType type = Component_Default;

	void SetParent(GameObject* parent = nullptr);
	GameObject* GetParent() const;

protected:

	
	bool active = true;
	GameObject* parent = nullptr;

};



#endif
