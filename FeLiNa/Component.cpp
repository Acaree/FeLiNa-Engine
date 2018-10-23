#include "Component.h"
#include "GameObject.h"


Component::Component(GameObject* parent)
{
	this->parent = parent;
}

Component::~Component()
{

}

void Component::Update(float dt)
{

}

void Component::Draw()
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
