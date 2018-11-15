#ifndef _MODULEFILESYSTEM_
#define _MODULEFILESYSTEM_

#include "Module.h"

enum FILE_TYPE
{
	MESH_FILE,
	MATERIAL_FILE,
	UKNOWN_FILE
};

struct PHYSFS_Stat;

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem(Application* app, bool start_enabled = true);
	~ModuleFileSystem();

	FILE_TYPE FindTypeFile(const char* file);

	FILE_TYPE FindOwnTypeFile(const char* file);

	uint Load(const char* path, char** buffer) const;

	void CreateFolder(const char* new_folder);

	char* SaveFile(char* buffer, uint size, std::string& outputFileName, FILE_TYPE file);

	uint SaveBufferData(char* buffer,const char* filePath, uint size);

	char* MoveFileToAssets(char* path);

	const char** GetAllFilesFromDir(const char* dir) const;

	bool FindNewAssetsFiles(const char* directory, char* file);
	
	bool isDirectory(const char* file) const;

	void RemoveAllDependencies(char* file_path);
	void GetPhysfsStats(const char* filename, PHYSFS_Stat& stat );
	void FreeEnumeratedFiles( const char** dir);
	bool ExistFile(const char* file); // TO REVISE: REPLACE ALL PHYSFS_EXIST TO THIS FUNCTION

private:
	std::string tmp_string;
};


#endif