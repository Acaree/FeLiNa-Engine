#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h"
#include "ComponentTexture.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Component_Mesh;
}

ComponentMesh::~ComponentMesh()
{

}


void ComponentMesh::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

Mesh* ComponentMesh::GetMesh() const
{
	return mesh;
}