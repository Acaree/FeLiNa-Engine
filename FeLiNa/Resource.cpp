#include "Resource.h"
#include "mmgr/mmgr.h"

Resource::Resource(uint uid, RESOURCE_TYPE type)
{
	this->uid = uid;
	this->type = type;
}

Resource::~Resource()
{
}

RESOURCE_TYPE Resource::GetType() const
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
	return loaded == 0;
}

uint Resource::LoadToMemory()
{
	if (isLoadedToMemory())
		LoadInMemory();

	return loaded++;
}

uint Resource::EraseToMemory()
{
	loaded--;

	if (isLoadedToMemory())
		EraseInMemory();

	return loaded;
}

uint Resource::CountReferences() const
{
	return loaded;
}

void Resource::SetExportedFile(const char* exported_file)
{
	this->exported_file = exported_file;
}