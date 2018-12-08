#include "ResourceScript.h"


ResourceScript::ResourceScript(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{

}

ResourceScript::~ResourceScript()
{

}

bool ResourceScript::LoadInMemory()
{
	graph = new NodeGraph(uid);
	
	return true;
}

bool ResourceScript::EraseInMemory()
{
	RELEASE(graph);
	return true;
}