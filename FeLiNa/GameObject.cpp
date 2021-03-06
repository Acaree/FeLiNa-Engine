#include "GameObject.h"
#include "Component.h"
#include "Application.h"
#include "ImGui/imgui.h"
#include "ModuleScene.h"
#include "Glew/include/glew.h"
#include "ComponentMesh.h"
#include "ComponentTransform.h"
#include "ModuleRenderer3D.h"
#include "Quadtree.h"
#include "ModuleResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ComponentMaterial.h"
#include "ComponentScript.h"
#include "ComponentCamera.h"
#include "ComponentSpeed.h"
#include "mmgr/mmgr.h"

GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;

	if (parent != nullptr)
		parent->childrens.push_back(this);

	bounding_box.SetNegativeInfinity();

	//Set default name
	strcpy(name, "No Name");

#ifndef GAME_MODE
	uid = App->GenerateRandomNumber();
#endif
}


GameObject::~GameObject()
{

	for (uint i = 0; i < childrens.size(); ++i)
	{
		RELEASE(childrens[i]);

	}
	childrens.clear();

	if (transform != nullptr)
		RELEASE(transform);
	if (mesh != nullptr)
		RELEASE(mesh);
	if (material != nullptr)
		RELEASE(material);
	if (camera != nullptr)
		RELEASE(camera);
	if (speed != nullptr)
		RELEASE(speed);
	if (script != nullptr)
		RELEASE(script);

	parent = nullptr;
}

void GameObject::Update(float dt)
{
	
	for (int i = 0; i < childrens.size(); ++i)
		childrens[i]->Update(dt);

	for (int i = 0; i < components.size(); ++i)
		components[i]->Update(dt);
}

bool GameObject::CleanUp()
{
	for (uint i = 0; i < childrens.size(); ++i)
	{

		RELEASE(childrens[i]);

	}
	childrens.clear();

	if (transform != nullptr)
		RELEASE(transform);
	if (mesh != nullptr)
		RELEASE(mesh);
	if (material != nullptr)
		RELEASE(material);
	if (camera != nullptr)
		RELEASE(camera);
	if (speed != nullptr)
		RELEASE(speed);
	if (script != nullptr)
		RELEASE(script);

	components.clear();

	return true;
}

//Clean all data but not the object
void GameObject::CleanData()
{
	for (std::vector<GameObject*>::const_iterator it = childrens.begin(); it != childrens.end(); ++it)
	{
		(*it)->CleanData();
	}
	childrens.clear();

	if (transform != nullptr)
		RELEASE(transform);
	if (mesh != nullptr)
		RELEASE(mesh);
	if (material != nullptr)
		RELEASE(material);
	if (camera != nullptr)
		RELEASE(camera);
	if (script != nullptr)
		RELEASE(script);
	if (speed != nullptr)
		RELEASE(speed);

	components.clear();

}


void GameObject::SetName(const char* name)
{
	strcpy_s(this->name, 100, name);
}

char* GameObject::GetName() const
{
	return (char*)name;
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
	child->SetParent(this);
}

GameObject* GameObject::GetChild(uint position) const
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

Component* GameObject::AddComponent(const ComponentType type)
{
	Component* component = nullptr;

	switch (type)
	{
	case Component_Transform:
		 transform = new ComponentTransform(this);
		 component = transform;
		break;
	case Component_Mesh:
		mesh = new ComponentMesh(this);
		mesh->SetUID(App->GenerateRandomNumber());
		component = mesh;
		break;
	case Component_Material:
		material = new ComponentMaterial(this);
		material->SetUID( App->GenerateRandomNumber());
		component = material;
		break;
	case Component_Camera:
		camera = new ComponentCamera(this);
		component = camera;
		break;
	case Component_Script:
		script = new ComponentScript(this);
		component = script;
		break;
	case Component_Speed:
		speed = new ComponentSpeed(this);
		component = speed;
		break;

	case Component_Default:
		break;
	default:
		break;
	}

	if (component != nullptr)
	{
		components.push_back(component);
		component->SetParent(this);
	}
	return component;
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

	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		GameObject* go = this;
		ImGui::SetDragDropPayload("Hierarchy_nodes", &go, sizeof(GameObject));
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_nodes"))
		{
			GameObject* tmp = *(GameObject**)payload->Data;
			tmp->GetParent()->ExtractChildrenFromList(tmp);
			AddChildren(tmp);

		}
		ImGui::EndDragDropTarget();
	}

	if (ImGui::IsItemClicked(0))//Left click
	{
		GameObject* go = App->scene->GetSelectedGameObject();

		if (go != nullptr)
		{
			go->SetSelected(false);
		}

		App->scene->SetSelectedGameObject(this);
		SetSelected(true);
	}

	if (ImGui::BeginPopupContextItem("Create"))
	{
		ShowGameObjectOptions();
	
		ImGui::EndPopup();
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

void GameObject::SetSelected(const bool selected)
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

	if (ImGui::TreeNodeEx("Properties:"))
	{

		if (ImGui::Checkbox("Static", &static_object))
		{
			if (static_object)
				App->scene->static_go.push_back(this);
				
			App->scene->need_update_quadtree = true;
		}

		ImGui::InputText("##name", name, DEFAULT_BUF_SIZE);

		ImGui::TreePop();
	}

	for (int i = 0; i < components.size(); ++i)
	{
		ImGui::PushID(i);
		components[i]->DrawInspector();
		ImGui::PopID();
	}

}


void GameObject::DrawBoundingBox()
{
	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glColor4f(0.25f, 1.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(bounding_box.Edge(i).a.x, bounding_box.Edge(i).a.y, bounding_box.Edge(i).a.z);
		glVertex3f(bounding_box.Edge(i).b.x, bounding_box.Edge(i).b.y, bounding_box.Edge(i).b.z);
	}
	glEnd();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	
	for (uint i = 0; i < childrens.size(); ++i)
	{
		childrens[i]->DrawBoundingBox();
	}

}


Component* GameObject::GetComponent(const ComponentType type)
{
	Component* component = nullptr;

	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->GetComponentType() == type)
		{
			component = components[i];
			break;
		}
	}

	return component;
}

void GameObject::RecalculateBoundingBox()
{
	bounding_box.SetNegativeInfinity();


	if (mesh != nullptr)
	{
		ResourceMesh* res_mesh =  (ResourceMesh*)App->resource_manager->Get(mesh->GetUID());
		bounding_box.Enclose((const math::float3*)res_mesh->vertices, res_mesh->num_vertices);
	}

	if (transform != nullptr)
	{
		math::OBB obb;
		obb.SetFrom(bounding_box);
		obb.Transform(transform->GetTransformMatrix());

		if (obb.IsFinite())
			bounding_box = obb.MinimalEnclosingAABB();
	}

	for (uint i = 0; i < childrens.size(); ++i)
		childrens[i]->RecalculateBoundingBox();
}

void GameObject::SetActive(const bool active)
{
	this->active = active;
}

bool GameObject::IsActive() const
{
	return active;
}

math::AABB GameObject::GetAABB() const
{
	return bounding_box;
}

void GameObject::OnSave(JSON_Object* obj)
{

	json_object_set_string(obj, "name", name);

	json_object_set_number(obj, "uid", uid);

	if(parent != nullptr)
		json_object_set_number(obj, "uid parent", parent->uid);

	JSON_Value* arr_components = json_value_init_array();
	JSON_Array* components_array = json_value_get_array(arr_components);

	for (uint i = 0; i < components.size(); ++i)
	{
		JSON_Value* new_value = json_value_init_object();
		JSON_Object* obj_value = json_value_get_object(new_value);

		components[i]->OnSave(obj_value);
		json_array_append_value(components_array, new_value);
	}

	json_object_set_value(obj, "Components", arr_components);

	//RECALCULATE BOUNDING BOX?

}

void GameObject::OnLoad(JSON_Object* obj)
{

	strcpy(name, (char*)json_object_get_string(obj, "name"));

	uid = json_object_get_number(obj, "uid");

	JSON_Array* components_array = json_object_get_array(obj, "Components");
	JSON_Object* obj_components;

	for (int i = 0; i < json_array_get_count(components_array); ++i) {

		obj_components = json_array_get_object(components_array, i);
		Component* newComponent = AddComponent((ComponentType)(int)json_object_get_number(obj_components, "type"));

		if(newComponent != nullptr)
			newComponent->OnLoad(obj_components);
	}

}

GameObject* GameObject::SearchParentForUID(const uint parent_uid)
{
	GameObject* go = nullptr;
	if (parent_uid == uid)
		return this;
	else
	{
		GameObject* parent_object = nullptr;

		for (uint i = 0; i < childrens.size(); ++i)
		{
			parent_object = childrens[i]->SearchParentForUID(parent_uid);
			
			if (parent_object != nullptr)
			{
				go = parent_object;
			}
		}

		return go;

	}
}

GameObject* GameObject::SearchGOForUID(const uint uid)
{
	GameObject* go = nullptr;

	if (this->uid == uid)
		return this;
	else
	{
		for (uint i = 0; i < childrens.size(); ++i)
		{
			go = childrens[i]->SearchGOForUID(uid);

			if (go != nullptr)
				break;
		}
	}

	return go;
}

void GameObject::ShowGameObjectOptions()
{

	static bool is_camera = false;

	if (camera == nullptr)
	{
		is_camera = true;
	}

	if (ImGui::MenuItem("Delete",NULL,false, is_camera))
	{
		to_delete = true;
	}
	

}

void GameObject::DeleteAllComponents()
{

	RELEASE(transform);
	RELEASE(mesh);
	RELEASE(material);
	RELEASE(camera);

	components.clear();
}

void GameObject::DeleteChildren(GameObject* go)
{
	
	std::vector<GameObject*>::iterator it = childrens.begin();

	for (uint i = 0; i < childrens.size(); ++i)
	{
		if (childrens[i]->uid == go->uid)
		{
			if (childrens[i]->to_delete) {
				GameObject* tmp = childrens[i];

				RELEASE(tmp);
				childrens.erase(it);
				tmp = nullptr;

				break;
			}
		}
		it++;

	}


}

//this don't delete the child pointer, as DeleteChildren does
void GameObject::ExtractChildrenFromList(GameObject* go) { 

	std::vector<GameObject*> tmp;
	std::vector<GameObject*>::iterator it = childrens.begin();

	for (uint i = 0; i < childrens.size(); ++i)
	{

		if (strcmp(childrens[i]->GetName(), go->name) != 0)
		{
			tmp.push_back(childrens[i]);

		}
		it++;

	}

	childrens = tmp;
}

void GameObject::SetInvalidateResource(const Resource* resource)
{
	if (resource != nullptr)
	{

		if (mesh != nullptr)
		{
			if (mesh->GetUID() == resource->uid)
			{
				mesh->SetUID(0);
			}
		}

		if (material != nullptr)
		{
			if (material->GetUID() == resource->uid)
			{
				material->SetUID(0);
			}
		}

		for (uint i = 0; i < childrens.size(); ++i)
		{
			childrens[i]->SetInvalidateResource(resource);
		}

	}

}

GameObject* GameObject::SearchMainCamera(GameObject* go)
{
	GameObject* main_camera = nullptr;

	if (go->camera != nullptr)
	{
		main_camera = go;
	}
	else
	{
		for (uint i = 0; i < childrens.size(); ++i)
		{
			main_camera = childrens[i]->SearchMainCamera(childrens[i]);

			if (main_camera != nullptr)
				break;
		}

	}

	return main_camera;


}

