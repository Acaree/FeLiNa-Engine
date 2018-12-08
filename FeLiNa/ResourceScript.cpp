#include "ResourceScript.h"


ResourceScript::ResourceScript(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{

}

ResourceScript::~ResourceScript()
{

}

bool ResourceScript::LoadInMemory()
{
	graph = new NodeGraph(uid, exported_file.substr(0,exported_file.find(EXTENSION_SCRIPT)).c_str());
	
	graph->LoadGraph(file.c_str());

	return true;
}

bool ResourceScript::EraseInMemory()
{
	RELEASE(graph);
	return true;
}