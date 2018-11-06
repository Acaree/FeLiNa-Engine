#include "ResourceMesh.h"


ResourceMesh::ResourceMesh(uint uid, ResourceType type) : Resource(uid,type)
{
}

ResourceMesh::~ResourceMesh()
{
}

bool ResourceMesh::LoadInMemory()
{
	bool ret = false;

	//We load the mesh and bind the buffers or only the buffers :/

	return ret;
}

bool ResourceMesh::EraseInMemory()
{
	bool ret = false;

	//Same than load.

	return ret;


}