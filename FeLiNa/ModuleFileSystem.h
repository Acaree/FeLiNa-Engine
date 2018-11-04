#ifndef _MODULEFILESYSTEM_
#define _MODULEFILESYSTEM_

#include "Module.h"


class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();

	const char* GetNameFile(const char* path) const;


	uint Load(const char* path, char** buffer) const;

	uint SaveTexture(char* buffer, uint size, std::string& outputFileName);

	uint SaveBufferData(char* buffer,const char* filePath, uint size);

	const char** GetAllFilesFromDir(const char* dir) const;
	
	bool isDirectory(const char* file) const;
private:

};


#endif