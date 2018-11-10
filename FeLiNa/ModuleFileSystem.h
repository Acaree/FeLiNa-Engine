#ifndef _MODULEFILESYSTEM_
#define _MODULEFILESYSTEM_

#include "Module.h"

enum FILE_TYPE
{
	MESH_FILE,
	MATERIAL_FILE,
	UKNOWN_FILE
};

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();

	FILE_TYPE FindTypeFile(const char* file);

	FILE_TYPE FindOwnTypeFile(const char* file);

	uint Load(const char* path, char** buffer) const;

	char* SaveFile(char* buffer, uint size, std::string& outputFileName, FILE_TYPE file);

	uint SaveBufferData(char* buffer,const char* filePath, uint size);

	char* MoveFileToAssets(char* path);

	const char** GetAllFilesFromDir(const char* dir) const;

	bool FindNewAssetsFiles(const char* directory, std::string& file);
	
	bool isDirectory(const char* file) const;
private:

};


#endif