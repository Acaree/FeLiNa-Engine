#ifndef _RESOURCE_MESH_
#define RESOURCE_MESH_

#include "Resource.h"

#include "MeshImporter.h"


class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint uid, RESOURCE_TYPE type);
	~ResourceMesh();

	bool LoadInMemory();
	bool EraseInMemory();


public:

	float* vertices = nullptr;
	uint num_vertices = 0;
	uint id_vertices = 0;

	uint* indices = nullptr;
	uint num_indices = 0;
	uint id_indices = 0;

	float* uv = nullptr;
	uint num_uv = 0;
	uint id_uv = 0;

};




#endif
