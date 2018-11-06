#ifndef _RESOURCE_MESH_
#define RESOURCE_MESH_

#include "Resource.h"

#include "MeshImporter.h"

class ResourceMesh : public Resource
{
public:
	ResourceMesh(uint uid, ResourceType type);
	~ResourceMesh();

	bool LoadInMemory();
	bool EraseInMemory();



private:

	Mesh mesh;

};




#endif
