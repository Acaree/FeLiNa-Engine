#include "ResourceMesh.h"
#include "Glew/include/glew.h"
#include <gl/GL.h>
#include "mmgr/mmgr.h"

ResourceMesh::ResourceMesh(uint uid, RESOURCE_TYPE type) : Resource(uid,type)
{

}

ResourceMesh::~ResourceMesh()
{	
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(uv);
}

bool ResourceMesh::LoadInMemory()
{
	bool ret = false;

	glGenBuffers(1, (GLuint*) &(id_vertices));
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * num_vertices, vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(id_indices));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * num_indices, indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(id_uv));
	glBindBuffer(GL_ARRAY_BUFFER, id_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * num_uv, uv, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return ret;
}

bool ResourceMesh::EraseInMemory()
{
	bool ret = false;

	glDeleteBuffers(1, (GLuint*) &(id_vertices));
	glDeleteBuffers(1, (GLuint*) &(id_indices));
	glDeleteBuffers(1, (GLuint*) &(id_uv));


	return ret;
}

