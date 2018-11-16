#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "MeshImporter.h"
#include "ImGui/imgui.h"
#include "Resource.h"
#include "MeshImporter.h"
#include "Application.h"
#include "ResourceManager.h"
#include "ResourceMesh.h"
#include "mmgr/mmgr.h"
ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Component_Mesh;
}

ComponentMesh::~ComponentMesh()
{
	if (uid != 0)
	{
		ResourceMesh* resource_mesh = (ResourceMesh*)App->resource_manager->Get(uid);
		if(resource_mesh != nullptr)
			resource_mesh->EraseToMemory();
	}
}


void ComponentMesh::SetUID(uint uid)
{
	this->uid = uid;
}

uint ComponentMesh::GetUID() const
{
	return uid;
}

void ComponentMesh::DrawInspector()
{
	if (ImGui::TreeNodeEx("Mesh"))
	{
		if (uid != 0)
		{
			ResourceMesh* resource = (ResourceMesh*)App->resource_manager->Get(uid);

			if (resource != nullptr)
			{

				ImGui::Text("Refernce counting: %i", resource->loaded);

				ImGui::Text("Indices: %i", resource->num_indices);
				ImGui::Text("Vertices: %i", resource->num_vertices);
				ImGui::Text("Uv's: %i", resource->num_uv);
				ImGui::Text("Triangles: %i", resource->num_vertices / 3);
			}
			else
				ImGui::Text("Invalid Mesh");
		}
		else
		{
			ImGui::Text("Invalid Mesh");
		}
		ImGui::TreePop();
	}
}


void ComponentMesh::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_number(obj, "UID", uid);
}

void ComponentMesh::OnLoad(JSON_Object* obj)
{
	this->uid = json_object_get_number(obj, "UID");
	if(uid != 0)
	{
		Resource* resource = App->resource_manager->Get(uid);

		if(resource != nullptr)
			resource->LoadToMemory();
		
	}
}