#include "ResourceScript.h"


ResourceScript::ResourceScript(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{

}

ResourceScript::~ResourceScript()
{

}

bool ResourceScript::LoadInMemory()
{
	graph = new NodeGraph();
	return true;
}

bool ResourceScript::EraseInMemmory()
{
	RELEASE(graph);
	return true;
}