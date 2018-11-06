#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceManager.h"
#include "Resource.h"


ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	resources.clear();

}

//The file need to load in ModuleFileSystem
uint ResourceManager::Find(const char* file) const
{
	
	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
		if (strcmp(it->second->GetExportedFile(), file) == 0)
			return it->first;

	return 0;
}

uint ResourceManager::ImportFile(const char* new_file)
{
	uint ret = 0;

	ret = Find(new_file);

	if (ret == 0)
	{
		//std::string file = App->fs-> NEED A FUNCTION.
		

	}

	return ret;
}

const Resource* ResourceManager::Get(uint uid)
{
	Resource* res = nullptr;

	std::map<uint, Resource*>::iterator it= resources.find(uid);

	if (it != resources.end())
		res = it->second;

	return res;
}

Resource* ResourceManager::CreateNewResource(ResourceType type)
{
	Resource* res = nullptr;

	if (type != ResourceType::Resource_Default)
	{
		uint uid = App->random->Int();

		switch (type)
		{
		case Resource_Mesh:
			// res = new resoucemesh
			break;
		case Resource_Material:
			//res = new resourcematerial
			break;
		default:
			break;
		}

		if (res != nullptr)
			resources[uid] = res;


	}

	return res;
}