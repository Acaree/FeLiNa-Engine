#ifndef _MODULEFILESYSTEM_
#define _MODULEFILESYSTEM_

#include "Module.h"

enum FILE_TYPE
{
	MESH_FILE,
	MATERIAL_FILE
};

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();

	const char* GetNameFile(const char* path) const;


	uint Load(const char* path, char** buffer) const;

	char* SaveFile(char* buffer, uint size, std::string& outputFileName, FILE_TYPE file);

	uint SaveBufferData(char* buffer,const char* filePath, uint size);

	const char** GetAllFilesFromDir(const char* dir) const;
	
	bool isDirectory(const char* file) const;
private:

};


#endif