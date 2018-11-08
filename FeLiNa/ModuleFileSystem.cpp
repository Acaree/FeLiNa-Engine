#include "ModuleFileSystem.h"
#include "Application.h"
#include "PhysFS/physfs.h"

#include "mmgr/mmgr.h"
#include <string>

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	PHYSFS_init(nullptr);

	if (PHYSFS_setWriteDir(".") == 0) // create a directory (if it's point it take game as base directory)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	if (PHYSFS_mount("./Assets/", "Assets", 1)==0) { //Add paths to physfs to search throught
	
		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}

	PHYSFS_mkdir("Library/Meshes");
	PHYSFS_mkdir("Library/Materials");

	if (PHYSFS_mount("./Library/", "Library", 1) == 0) { 

		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}

	if (PHYSFS_mount("./Library/Meshes/", "Meshes", 1) == 0) {

		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}

	if (PHYSFS_mount("./Library/Materials/", "Materials", 1) == 0) {

		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}

	
	
}

ModuleFileSystem::~ModuleFileSystem()
{

	PHYSFS_deinit();

}


uint ModuleFileSystem::Load(const char* filePath, char** buffer) const {
	
	uint count = 0;

	bool file_exists = PHYSFS_exists(filePath);

	if (file_exists) {
		
		PHYSFS_File* file = PHYSFS_openRead(filePath);

		if (file != nullptr) {

			PHYSFS_sint64 size = PHYSFS_fileLength(file);

			if (size > 0) {
				
				*buffer = new char[size];
				count = PHYSFS_readBytes(file, *buffer, size);

				if (count == size)
				{
					LOG("FILE SYSTEM: Read %u bytes from file '%s'", count, filePath);
					
				}
				else
				{
					RELEASE(buffer);
					LOG("FILE SYSTEM: Could not read from file '%s'. ERROR: %s", filePath, PHYSFS_getLastError());
				}

				if (PHYSFS_close(file) == 0)
					LOG("FILE SYSTEM: Could not close file '%s'. ERROR: %s", filePath, PHYSFS_getLastError());
			}
		}

		else {

			PHYSFS_sint64 size = PHYSFS_fileLength(file);

			if (size > 0) {

				*buffer = new char[size];
				count = PHYSFS_readBytes(file, *buffer, size);

				if (count == size)
				{
					LOG("FILE SYSTEM: Read %u bytes from file '%s'", count, filePath);

				}
				else
				{
					RELEASE(buffer);
					LOG("FILE SYSTEM: Could not read from file '%s'. ERROR: %s", filePath, PHYSFS_getLastError());
				}

				if (PHYSFS_close(file) == 0)
					LOG("FILE SYSTEM: Could not close file '%s'. ERROR: %s", filePath, PHYSFS_getLastError());
			}
		}
	}
	else
		LOG("FILE SYSTEM: Could not load file '%s' to read because it doesn't exist", filePath);

	return count;
}

const char* ModuleFileSystem::GetNameFile(const char* path) const
{
	//Erase folder and extension from path

	std::string file_name = path;
	
	
	file_name = file_name.substr(file_name.find_last_of("//") + 1, file_name.size());
	file_name = file_name.substr(0, file_name.find_last_of("."));

	char* result = new char[file_name.size() + 1];

	strcpy_s(result, file_name.size() + 1, file_name.data());

	//file_name.clear();

	return result;
}

char* ModuleFileSystem::SaveFile(char* buffer, uint size, std::string& output_file, FILE_TYPE file)
{
	

	//COPY NAME
	char new_name[DEFAULT_BUF_SIZE];
	sprintf_s(new_name, DEFAULT_BUF_SIZE, output_file.data());
	output_file = new_name;

	char new_path[DEFAULT_BUF_SIZE];

	//COPY PATH
	switch (file)
	{
	case MESH_FILE:
		sprintf_s(new_path, DEFAULT_BUF_SIZE, "Library/Meshes/%s.%s", new_name, EXTENSION);
		break;
	case MATERIAL_FILE:
		sprintf_s(new_path, DEFAULT_BUF_SIZE, "Library/Materials/%s.%s", new_name, "dds");
		break;
	default:
		break;
	}


	SaveBufferData(buffer, new_path, size);

	return new_path;
}

uint ModuleFileSystem::SaveBufferData(char* buffer, const char* file_path, uint size)
{
	uint count = 0;

	const char* file_name = GetNameFile(file_path);

	bool exists = PHYSFS_exists(file_path);

	PHYSFS_File* file = nullptr;

	if (!exists)
		file = PHYSFS_openAppend(file_path);
	else
		file = PHYSFS_openWrite(file_path);

	if (file != nullptr)
	{
		count = PHYSFS_writeBytes(file, (const void*)buffer, size);

		if (count == size)
		{
			if (exists)
			{
				LOG("FILE SYSTEM: CREATE ");
			}
			else
				LOG("FILE SYSTEM: FAIL");
		}
		else
			LOG("FILE SYSTEM: Could not write to file '%s'. ERROR: %s", file_name, PHYSFS_getLastError());

		if (PHYSFS_close(file) == 0)
			LOG("FILE SYSTEM: Could not close file '%s'. ERROR: %s", file_name, PHYSFS_getLastError());
	}
	else
		LOG("FILE SYSTEM: Could not open file '%s' to write. ERROR: %s", file_name, PHYSFS_getLastError());

	return count;
}

const char** ModuleFileSystem::GetAllFilesFromDir(const char* dir) const
{
	return (const char**)PHYSFS_enumerateFiles(dir);
}

bool ModuleFileSystem::isDirectory(const char* file) const
{
	return PHYSFS_isDirectory(file);
}

FILE_TYPE ModuleFileSystem::FindTypeFile(const char* file)
{
	FILE_TYPE file_type = FILE_TYPE::UKNOWN_FILE;

	std::string file_extension = file;

	file_extension = file_extension.erase(0, file_extension.find_last_of("."));

	
	if (strcmp(file_extension.c_str(), ".felina") == 0 )
	{
		file_type = FILE_TYPE::MESH_FILE;
	}
	else if( strcmp(file_extension.c_str(), ".dds") == 0 )
	{
		file_type = FILE_TYPE::MATERIAL_FILE;
	}

	return file_type;
}


bool ModuleFileSystem::FindNewAssetsFiles(const char* directory, std::string & new_file)
{
	bool ret = false;
	
	//Really similar function that gui::RecurssiveShowAssets...

	new_file.append(directory);
	
	const char** files_array = GetAllFilesFromDir(directory);

	for (const char** file = files_array; *file != nullptr; ++file)
	{
		if (isDirectory(*file))
		{
			if (FindNewAssetsFiles(*file, new_file))
				if (int position = new_file.rfind(*file) != std::string::npos) //That condition work as espected?
					new_file = new_file.substr(0, position);
		}
		else
		{
			std::string extension = *file;
			extension = extension.substr(extension.find_last_of("."));

			//TO REVISE THIS JSON HAS A .sceneFelina
			if (strcmp(extension.data(), "json") == 0 || strcmp(extension.data(), "meta") == 0)
				continue;

			char meta[DEFAULT_BUF_SIZE];

			strcpy(meta, new_file.data());
			strcat(meta, *file);
			strcat(meta, extension.data());

			if (!PHYSFS_exists(meta))
			{
				new_file.append(*file);
				ret = true;
				break;
			}
		}
	}

	return ret;
}

char* ModuleFileSystem::MoveFileToAssets(char* path) {

	std::string filename = path;
	filename.erase(0, filename.find_last_of("\\") + 1);

	char* ret = new char[DEFAULT_BUF_SIZE];

	sprintf_s(ret, DEFAULT_BUF_SIZE, "Assets/%s", filename.data());

	CopyFile(path, ret, 0);

	return ret;
}