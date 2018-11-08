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

	void SetTexture(Texture* texture);
	Texture* GetTexture() const;

	bool LoadInMemory();
	bool EraseInMemory();

private:
	Texture* texture;
};



#endif
