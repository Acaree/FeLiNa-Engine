#ifndef _RESOURCE_MATERIAL_
#define _RESOURCE_MATERIAL_

#include "Resource.h"
#include "MaterialImporter.h"

class ResourceMaterial : public Resource
{
public:
	ResourceMaterial(uint uid, ResourceType type);
	~ResourceMaterial();

	bool LoadInMemory();
	bool EraseInMemory();

private:
	Texture_Importer texture;
};



#endif
