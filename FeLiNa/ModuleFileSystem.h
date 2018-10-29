#ifndef _MODULEFILESYSTEM_
#define _MODULEFILESYSTEM_

#include "Module.h"


class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();

	bool SaveFile(std::string& output, const void* buffer, uint size, const char* path, const char* prefix, const char* extension);
	uint PhysfsSave (const char* file_path, const void* buffer, unsigned int size, bool append = false) const;


private:

};


#endif