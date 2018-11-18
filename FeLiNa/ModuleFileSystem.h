#ifndef _MODULEFILESYSTEM_
#define _MODULEFILESYSTEM_

#include <list>
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

	update_status PreUpdate(float dt);

	FILE_TYPE FindTypeFile(const char* file);

	FILE_TYPE FindOwnTypeFile(const char* file);

	uint Load(const char* path, char** buffer) const;

	void CreateFolder(const char* new_folder);
	void DeleteFolderandContainedFiles(const char* folder_to_remove);

	char* SaveFile(char* buffer, uint size, std::string& outputFileName, FILE_TYPE file);

	uint SaveBufferData(char* buffer,const char* filePath, uint size);

	char* MoveFileToAssets(char* path);

	const char** GetAllFilesFromDir(const char* dir) const;
	bool RecursiveFindFileExistInAssets(const char* dir, const char* file_name, std::string& path);
	bool isDirectory(const char* file) const;

	void RemoveAllDependencies(char* file_path);
	void GetPhysfsStats(const char* filename, PHYSFS_Stat& stat );
	void FreeEnumeratedFiles( const char** dir);
	bool ExistFile(const char* file); 
	uint GetLastModificationTime(const char* dir);

	void GetAllAssetsFiles(const char* dir, std::string path);
	void CheckAllAssetsFiles();

	void FileDelete(const char* path);

private:
	std::list<std::string> assets_files;
	std::string tmp_string;

public:
	bool refresh_now = false;
};


#endif