#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"

Component::Component(GameObject* parent)
{
	this->parent = parent;
	uid = App->random->Int();
}

Component::~Component()
{

}

void Component::Update(float dt)
{

}

void Component::DrawInspector()
{

}

void Component::CleanUp()
{
	delete parent;
	parent = nullptr;
}

void Component::SetComponentType(ComponentType type)
{
	this->type = type;
}

ComponentType Component::GetComponentType() const
{
	return type;
}

void Component::SetActive(bool active)
{
	this->active = active;
}

bool Component::GetActive() const
{
	return active;
}

void Component::SetParent(GameObject* parent)
{
	this->parent = parent;
}

GameObject* Component::GetParent() const
{
	return parent;
}

void Component::OnLoad(JSON_Object* obj)
{

}

void Component::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", Component_Default);
}