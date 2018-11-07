#ifndef _RESORUCE_MANAGER_
#define _RESOURCE_MANAGER_


#include "Globals.h"

#include <map>

class Resource;
enum RESOURCE_TYPE;

class ResourceManager //: publc Module ??
{
public:

	ResourceManager();
	~ResourceManager();

	uint Find(const char* file) const;
	uint ImportFile(const char* new_file);
	const Resource* Get(uint uid);
	Resource* CreateNewResource(RESOURCE_TYPE type);
	

private:
	
	std::map<uint, Resource*> resources;
};




#endif
