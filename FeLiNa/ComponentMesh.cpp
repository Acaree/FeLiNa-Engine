#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h"
#include "ComponentTexture.h"
#include "ImGui/imgui.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Component_Mesh;
}

ComponentMesh::~ComponentMesh()
{
	mesh = nullptr;
}


void ComponentMesh::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

Mesh* ComponentMesh::GetMesh() const
{
	return mesh;
}

void ComponentMesh::DrawInspector()
{
	if (ImGui::TreeNodeEx("Mesh"))
	{
		ImGui::Text("Name: %s", mesh->name);
		ImGui::Separator();
		ImGui::Text("Indices: %i", mesh->num_indices);
		ImGui::Text("Vertices: %i", mesh->num_vertices);
		ImGui::Text("Uv's: %i", mesh->num_uv);
		ImGui::Text("Triangles: %i", mesh->num_vertices/3);

		ImGui::TreePop();
	}
}