#include "Component.h"
#include "GameObject.h"


Component::Component()
{

}

Component::Component(Component* component)
{
	this->type = component->GetComponentType();
	this->active = component->GetComponentType();
	this->parent = component->GetParent();
}

Component::Component(ComponentType type)
{
	this->type = type;
}

Component::Component(GameObject* parent, ComponentType type)
{
	this->parent = parent;
	this->type = type;
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
