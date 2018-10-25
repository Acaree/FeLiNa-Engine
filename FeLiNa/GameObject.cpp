#include "GameObject.h"
#include "Component.h"
#include "ImGui/imgui.h"


GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;

	if (parent != nullptr)
		parent->childrens.push_back(this);
}



GameObject::~GameObject()
{
	for (std::vector<GameObject*>::const_iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		(*it)->CleanUp();
	}
	childrens.clear();

	delete name;
	name = nullptr;

	delete parent;
	parent = nullptr;
}

void GameObject::Update(float dt)
{

	for (int i = 0; i < childrens.size(); ++i)
		childrens[i]->Update(dt);

	for (int i = 0; i < components.size(); ++i)
		components[i]->Draw();


}

bool GameObject::CleanUp()
{
	for (std::vector<GameObject*>::const_iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		(*it)->CleanUp();
	}
	childrens.clear();

	for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
	{
		(*it)->CleanUp();
	}
	childrens.clear();

	delete name;
	name = nullptr;

	delete parent;
	parent = nullptr;

	return true;
}


void GameObject::SetName(char* name)
{
	this->name = name;
	
}

char* GameObject::GetName() const
{
	return name;
}

void GameObject::SetActive(bool* active)
{
	this->active = active;
}

bool GameObject::GetActive() const
{
	return active;
}

void GameObject::AddChildren(GameObject* child)
{
	childrens.push_back(child);
}

GameObject* GameObject::GetChild(uint position)
{
	return childrens[position];
}

void GameObject::SetParent(GameObject* parent)
{
	this->parent = parent;
}

GameObject* GameObject::GetParent() const
{
	return parent;
}

void GameObject::SetComponent(Component* component)
{
	//need check if component exist - TO REVISION
	components.push_back(component);
}

bool GameObject::DeleteComponent(Component* component)
{
	bool ret = false;

	if (components.size() != 0)
	{

		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		{
			if ((*it) == component)
			{
				(*it)->CleanUp();
				components.erase(it);

				ret = true;
			}

		}

		components.shrink_to_fit();
	}

	return ret;
}

bool GameObject::DeleteComponent(ComponentType type)
{
	bool ret = false;

	if (components.size() != 0)
	{

		for (std::vector<Component*>::const_iterator it = components.begin(); it != components.end(); ++it)
		{
			if ((*it)->GetComponentType() == type)
			{
				(*it)->CleanUp();
				components.erase(it);

				ret = true;
			}

		}

		components.shrink_to_fit();
	}

	return ret;
}

uint GameObject::GetNumChildren() const
{
	return childrens.size();
}

void GameObject::ShowObjectHierarchy()
{
	
		if (GetNumChildren() != 0)
		{
			for (int i = 0; i < GetNumChildren(); ++i)
			{
				if (ImGui::TreeNodeEx(childrens[i]->GetName()))
				{
					childrens[i]->ShowObjectHierarchy();
					ImGui::TreePop();
				}
			}
		}
	
}

void GameObject::SetSelected(bool selected)
{
	this->selected = selected;
}

bool GameObject::IsSelected() const
{
	return selected;
}

void GameObject::ToggleSelected()
{
	if (selected)
		selected = false;
	else
		selected = true;
}