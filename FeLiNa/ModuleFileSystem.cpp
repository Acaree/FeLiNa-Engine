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

	if (PHYSFS_setWriteDir("Assets") == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	PHYSFS_mkdir("Phys_saves");
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