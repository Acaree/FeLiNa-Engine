#ifndef _RESOURCE_MATERIAL_
#define _RESOURCE_MATERIAL_

#include "Resource.h"
#include "MaterialImporter.h"
#include "ComponentTexture.h"


class ResourceMaterial : public Resource
{
public:

	ResourceMaterial(uint uid, RESOURCE_TYPE type);
	~ResourceMaterial();

	bool LoadInMemory();
	bool EraseInMemory();

public:
	uint id_texture = 0;
	uint widht = 0;
	uint height = 0;
};



#endif
