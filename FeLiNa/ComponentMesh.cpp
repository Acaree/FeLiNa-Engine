#include "ComponentMesh.h"
#include "Glew/include/glew.h"
#include "GameObject.h"
#include "ComponentTexture.h"

ComponentMesh::ComponentMesh(GameObject* parent) : Component(parent)
{
	type = Component_Mesh;
}

ComponentMesh::~ComponentMesh()
{

}

void ComponentMesh::Draw()
{

	ComponentTexture* texture_com = nullptr;

	for (int i = 0; i <= parent->components.size(); i++) {
		if (parent->components[i]->type == Component_Material) {
			texture_com = (ComponentTexture*)(parent->components[i]);
			break;
		}
	}

	glBindTexture(GL_TEXTURE_2D, texture_com->texture_id);

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uv);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices); 

	glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, 0);


}

void ComponentMesh::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
}