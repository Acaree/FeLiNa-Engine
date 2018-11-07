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

	void SetMesh(Mesh* mesh);
	Mesh* GetMesh() const;

private:

	Mesh* mesh;

};




#endif
