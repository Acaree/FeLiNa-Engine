#include "ResourceMesh.h"
#include "Glew/include/glew.h"
#include <gl/GL.h>
#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh(uint uid, RESOURCE_TYPE type) : Resource(uid,type)
{
	mesh = new Mesh;
}

ResourceMesh::~ResourceMesh()
{	
	
}

bool ResourceMesh::LoadInMemory()
{
	bool ret = false;

	glGenBuffers(1, (GLuint*) &(mesh->id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * mesh->num_vertices, mesh->vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(mesh->id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->num_indices, mesh->indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(mesh->id_uv));
	glBindBuffer(GL_ARRAY_BUFFER, mesh->id_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * mesh->num_uv, mesh->uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


	return ret;
}

bool ResourceMesh::EraseInMemory()
{
	bool ret = false;

	glDeleteBuffers(1, (GLuint*) &(mesh->id_vertices));
	glDeleteBuffers(1, (GLuint*) &(mesh->id_indices));
	glDeleteBuffers(1, (GLuint*) &(mesh->id_uv));

	if (mesh->indices != nullptr)
	{
		RELEASE_ARRAY(mesh->indices);
	}

	if (mesh->vertices != nullptr)
	{
		RELEASE_ARRAY(mesh->vertices);
	}

	if (mesh->uv != nullptr)
	{
		RELEASE_ARRAY(mesh->uv);

	}

	RELEASE_ARRAY(mesh->felina_path);
	RELEASE(mesh);

	return ret;


}

void ResourceMesh::SetMesh(Mesh* mesh)
{
	this->mesh = mesh;
}

Mesh* ResourceMesh::GetMesh() const
{
	return mesh;
}