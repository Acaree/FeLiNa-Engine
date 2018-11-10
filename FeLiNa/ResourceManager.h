#ifndef _RESORUCE_MANAGER_
#define _RESOURCE_MANAGER_

#include "Module.h"
#include "Globals.h"

#include <map>

class Resource;
enum RESOURCE_TYPE;

class ResourceManager: public Module 
{
public:

	ResourceManager(Application* app, bool start_enabled = true);
	~ResourceManager();

	bool Start();
	update_status PreUpdate(float dt);

	uint Find(const char* file) const;
	uint ImportFile(const char* new_file);
	Resource* Get(uint uid);
	Resource* CreateNewResource(RESOURCE_TYPE type);

	template<typename DATA>
	void SetResourceData(DATA* data, Resource* resource);

private:
	
	std::map<uint, Resource*> resources;
	float refresh_time = 0.0F;
	float time_to_refresh = 1.0F;
};


template<typename DATA>
inline void ResourceManager::SetResourceData(DATA* data, Resource* resource)
{
	RESOURCE_TYPE type = resource->GetType();

	switch (type)
	{
	case RESOURCE_MESH:
	{
		ResourceMesh* resource_mesh = (ResourceMesh*)resource;
		resource_mesh->SetMesh((Mesh*)data);
	}
		break;
	case RESOURCE_MATERIAL:
	{
		ResourceMaterial* resource_material = (ResourceMaterial*)resource;
		resource_material->SetTexture((Texture*)data);
	}
		break;
	case RESOURCE_DEFAULT:
		LOG("Can't assign resource data")
		break;
	default:
		break;
	}

}


#endif
