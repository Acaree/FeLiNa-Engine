#include "GameObject.h"
#include "Component.h"
#include "Application.h"
#include "ImGui/imgui.h"
#include "ModuleScene.h"
#include "Glew/include/glew.h"
#include "ComponentMesh.h"
#include "ModuleRenderer3D.h"

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

	if(bounding_box != nullptr)
		DrawBoundingBox();

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
	if (component != nullptr)
	{
		component->SetParent(this);
		components.push_back(component);
	}
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
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	bool node_open = false;

	if (selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (childrens.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

	if (ImGui::TreeNodeEx(name, flags))
		node_open = true;

	if (ImGui::IsItemClicked())
	{
		GameObject* go = App->scene->GetSelectedGameObject();

		if (go != nullptr)
		{
			go->SetSelected(false);
		}

		App->scene->SetSelectedGameObject(this);
		SetSelected(true);
	}

	if (node_open)
	{
		for (uint i = 0; i < childrens.size(); ++i)
		{
			ImGui::PushID(i);
			childrens[i]->ShowObjectHierarchy();
			ImGui::PopID();
		}

		ImGui::TreePop();
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

void GameObject::ShowObjectInspector()
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	bool node_open = false;

	if (selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	if (components.size() == 0)
		flags |= ImGuiTreeNodeFlags_Leaf;

}

void GameObject::AddBoundingBox(const Mesh* mesh)
{
	if (bounding_box == nullptr)
		bounding_box = new AABB();
	
	bounding_box->SetNegativeInfinity();
	bounding_box->Enclose((float3 *)mesh->vertices,mesh->num_vertices);
}

void GameObject::DrawBoundingBox()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(bounding_box->Edge(i).a.x, bounding_box->Edge(i).a.y, bounding_box->Edge(i).a.z);
		glVertex3f(bounding_box->Edge(i).b.x, bounding_box->Edge(i).b.y, bounding_box->Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

Component* GameObject::GetComponent(ComponentType type)
{
	Component* component = nullptr;

	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->type == type)
		{
			component = components[i];
			break;
		}
	}

	return component;
}