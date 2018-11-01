#include "ModuleFileSystem.h"
#include "Application.h"
#include "PhysFS/physfs.h"


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

	if (PHYSFS_mount("./Assets/Textures/", "Textures", 1) == 0) { //Add paths to physfs to search throught

		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}

	PHYSFS_mkdir("Assets/PhysfsSave");

	
}

ModuleFileSystem::~ModuleFileSystem()
{

	PHYSFS_deinit();

}


bool ModuleFileSystem::SaveFile(std::string& name, const void * buffer, uint size, const char * path, const char * prefix, const char * extension)
{
	char result[250];

	sprintf_s(result, 250, "%s/%s.%s", path, prefix, extension); //in test should be the function to generate a unique filename
	if (PhysfsSave(result, buffer, size) > 0)
	{
		name = result;
		return true;
	}
	return false;
}

uint ModuleFileSystem::PhysfsSave(const char* file_path, const void* buffer, unsigned int size, bool append) const {

	uint ret = 0;

	bool file_exists = PHYSFS_exists(file_path);

	PHYSFS_File* file = nullptr;

	if (file_exists) {
		file = PHYSFS_openWrite(file_path); //if file exists, it's opened with WriteMode
	}

	else {
		file = PHYSFS_openAppend(file_path); //if not, appendMode creates it
	}

	if (file != nullptr) {

		uint write = (uint)PHYSFS_write(file, buffer, 1, size);
		ret = write;

		if (write != size) {
			LOG("File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else {

			if (file_exists) {
				LOG("File %s overwritten", PHYSFS_getWriteDir());
			}

			else {
				LOG("New File created at %s", PHYSFS_getWriteDir());
			}

			if (PHYSFS_close(file) == 0)
				LOG("Error closing file %s: %s", file_path, PHYSFS_getLastError());
		}

	}

	else {
		LOG("Error Opening file %s: %s", file_path, PHYSFS_getLastError());
	}

	return ret;
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
				count = PHYSFS_read(file, *buffer, 1, size);

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
		else
			LOG("FILE SYSTEM: Could not open file '%s' to read. ERROR: %s", filePath, PHYSFS_getLastError());
	}
	else
		LOG("FILE SYSTEM: Could not load file '%s' to read because it doesn't exist", filePath);

	return count;
}

const char* ModuleFileSystem::GetNameFile(const char* path) const
{
	std::string file_name = path;
	
	//Erase the folder and extension
	file_name = file_name.substr(file_name.find_last_of("//") + 1, file_name.size());
	file_name = file_name.substr(0, file_name.find_last_of("."));

	char* result = new char[file_name.size() + 1];

	strcpy_s(result, file_name.size() + 1, file_name.data());


	return result;
}

uint ModuleFileSystem::SaveTexture(char* buffer, uint size, std::string& output_file)
{
	uint ret = 0;

	//COPY NAME
	char new_name[DEFAULT_BUF_SIZE];
	sprintf_s(new_name, DEFAULT_BUF_SIZE, output_file.data());
	output_file = new_name;

	//COPY PATH
	char new_path[DEFAULT_BUF_SIZE];
	sprintf_s(new_path, DEFAULT_BUF_SIZE, "Assets/PhysfsSave/%s.%s", new_name, EXTENSION);

	ret = SaveBufferData(buffer, new_path, size);

	return ret;
}

//  char* buffer, const char* filePath, uint size
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