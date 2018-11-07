#ifndef _RESOURCE_
#define _RESOURCE_

#include "Globals.h"
#include <string>

enum RESOURCE_TYPE
{
	RESOURCE_MESH = 0,
	RESOURCE_MATERIAL,
	RESOURCE_DEFAULT
};

class Resource
{
public:
	Resource(uint uid, RESOURCE_TYPE type);
	virtual ~Resource();

	RESOURCE_TYPE GetType() const;
	uint GetUID() const;
	const char* GetFile() const;
	const char* GetExportedFile() const;
	void SetExportedFile(const char* exported_file);
	bool isLoadedToMemory() const;
	uint LoadToMemory();
	uint CountReferences() const;

	virtual bool LoadInMemory() = 0;
	virtual bool EraseInMemory() = 0;
	/*
	Save
	Load
	*/
private:
	uint uid = 0;
	std::string file;
	std::string exported_file;

	RESOURCE_TYPE type = RESOURCE_TYPE::RESOURCE_DEFAULT;
	uint loaded = 0;
};



#endif
