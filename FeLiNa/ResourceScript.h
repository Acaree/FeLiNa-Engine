#ifndef _RESOURCE_SCRIPT_
#define _RESOURCE_SCRIPT_


#include "Resource.h"
#include "NodeGraph.h"


class ResourceScript : public Resource
{
public:
	ResourceScript(uint uid, RESOURCE_TYPE type);
	~ResourceScript();

	bool LoadInMemory();
	bool EraseInMemmory();

public:
	NodeGraph* graph = nullptr;
};


#endif