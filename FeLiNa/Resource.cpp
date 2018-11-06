#include "Resource.h"


Resource::Resource(uint uid, ResourceType type)
{
	this->uid = uid;
	this->type = type;
}

Resource::~Resource()
{
}

ResourceType Resource::GetType() const
{
	return type;
}

uint Resource::GetUID() const
{
	return uid;
}

const char* Resource::GetFile() const
{
	return file.c_str();
}

const char* Resource::GetExportedFile() const
{
	return exported_file.c_str();
}

bool Resource::isLoadedToMemory() const
{
	return loaded != 0;
}

uint Resource::LoadToMemory()
{
	if (!isLoadedToMemory())
		LoadInMemory();

	return loaded++;
}

uint Resource::CountReferences() const
{
	return loaded;
}
