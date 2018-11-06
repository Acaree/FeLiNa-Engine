#ifndef _RESOURCE_
#define _RESOURCE_

#include "Globals.h"
#include <string>

enum ResourceType
{
	Mesh = 0,
	Material,
	Default
};

class Resource
{
public:
	Resource(uint uid, ResourceType type);
	virtual ~Resource();

	ResourceType GetType() const;
	uint GetUID() const;
	const char* GetFile() const;
	const char* GetExportedFile() const;
	bool isLoadedToMemory() const;
	uint LoadToMemory();
	uint CountReferences() const;

	virtual bool LoadInMemory() = 0;

	/*
	Save
	Load
	*/
private:
	uint uid = 0;
	std::string file;
	std::string exported_file;

	ResourceType type = Default;
	uint loaded = 0;
};



#endif
