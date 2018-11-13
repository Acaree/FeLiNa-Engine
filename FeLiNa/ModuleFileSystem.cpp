#include "ModuleFileSystem.h"
#include "Application.h"
#include "PhysFS/physfs.h"
#include <string>

#include "mmgr/mmgr.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "File System";


	char* base_path = SDL_GetBasePath();
	PHYSFS_init(base_path);
	SDL_free(base_path);

	PHYSFS_init(nullptr);

	if (PHYSFS_setWriteDir(".") == 0) // create a directory (if it's point it take game as base directory)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	

	if (PHYSFS_mount("./Assets/", "Assets", 1)==0) { //Add paths to physfs to search throught
	
		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}

	const char** tmp = GetAllFilesFromDir("/Assets");

	for (const char** file = tmp; *file != nullptr; ++file)
	{
		// TO revise 
		std::string dir = "Assets/";
		dir += *file;
		PHYSFS_Stat* stat = new PHYSFS_Stat();
		PHYSFS_stat(dir.c_str(), stat);
		
		if(stat->filetype == PHYSFS_FileType::PHYSFS_FILETYPE_DIRECTORY)
			if(PHYSFS_mount(dir.c_str(), *file, 1) == 0)
				LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

		RELEASE(stat);
	}

	
	PHYSFS_mkdir("Assets/Settings");
	if (PHYSFS_mount("./Assets/Settings/", "Settings", 1) == 0) { //Add paths to physfs to search throught

		LOG("Physfs could not fin the path %s", PHYSFS_getLastError());

	}
	if (PHYSFS_mount("./Assets/Textures/", "Textures", 1) == 0) { //Add paths to physfs to search throught

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

void ModuleFileSystem::GetPhysfsStats(const char* file, PHYSFS_Stat & stats)
{
	PHYSFS_stat(file, &stats);
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
				
				RELEASE_ARRAY(buffer);
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

				RELEASE_ARRAY(buffer)
			}
		}
	}
	else
		LOG("FILE SYSTEM: Could not load file '%s' to read because it doesn't exist", filePath);

	return count;
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

	std::string tmp = file_path;
	tmp = tmp.substr(tmp.find_last_of("//") + 1, tmp.size());
	tmp = tmp.substr(0, tmp.find_last_of(".")).c_str();

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
			LOG("FILE SYSTEM: Could not write to file '%s'. ERROR: %s", tmp.c_str(), PHYSFS_getLastError());

		if (PHYSFS_close(file) == 0)
			LOG("FILE SYSTEM: Could not close file '%s'. ERROR: %s", tmp.c_str(), PHYSFS_getLastError());
	}
	else
		LOG("FILE SYSTEM: Could not open file '%s' to write. ERROR: %s", tmp.c_str(), PHYSFS_getLastError());

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

	
	if (strcmp(file_extension.c_str(), ".FBX") == 0 || strcmp(file_extension.c_str(), ".fbx") == 0 || strcmp(file_extension.c_str(), ".obj") == 0 || strcmp(file_extension.c_str(), ".OBJ") == 0)
	{
		file_type = FILE_TYPE::MESH_FILE;
	}
	else if( strcmp(file_extension.c_str(), ".dds") == 0 || strcmp(file_extension.c_str(), ".DDS") == 0 || strcmp(file_extension.c_str(), ".png") == 0 || strcmp(file_extension.c_str(), ".PNG") == 0 ||
		strcmp(file_extension.c_str(), ".jpg") == 0 || strcmp(file_extension.c_str(), ".JPG") == 0 || strcmp(file_extension.c_str(), ".tga") == 0 || strcmp(file_extension.c_str(), ".TGA") == 0)
	{
		file_type = FILE_TYPE::MATERIAL_FILE;
	}
	//else file type is unknown
	return file_type;
}


FILE_TYPE ModuleFileSystem::FindOwnTypeFile(const char* file)
{
	FILE_TYPE file_type = FILE_TYPE::UKNOWN_FILE;

	std::string file_extension = file;

	file_extension = file_extension.erase(0, file_extension.find_last_of("."));


	if (strcmp(file_extension.c_str(), ".felina") == 0)
	{
		file_type = FILE_TYPE::MESH_FILE;
	}
	else if (strcmp(file_extension.c_str(), ".dds") == 0)
	{
		file_type = FILE_TYPE::MATERIAL_FILE;
	}

	//else file type is unknown

	return file_type;
}


bool ModuleFileSystem::FindNewAssetsFiles(const char* directory, char* new_file)
{
	bool ret = false;

	//TO REVISE

	/*1- Check if is directory : true: recursive
	2-chech if are asociate the .meta in the soma folder: yes continue, no create*/

	char** files = PHYSFS_enumerateFiles(directory);
	char** file;

	for (file = files; file != nullptr; ++file)
	{
		if (PHYSFS_isDirectory(*file))
		{
			char* new_dir = new char[DEFAULT_BUF_SIZE];
			char* tmp_file = new char[DEFAULT_BUF_SIZE];

			strcpy(new_dir, *file);
			strcat(new_dir, "/");

			if (FindNewAssetsFiles(new_dir, tmp_file))
			{
				ret = true;

				strcpy(new_file, directory);


				strcat(new_file, tmp_file);
				RELEASE_ARRAY(tmp_file);
			}

			RELEASE_ARRAY(new_dir);
	
		}
		else
		{
			if (*file == nullptr)
			{
				RELEASE_ARRAY(new_file);
				break;
			}
			char* tmp = new char[DEFAULT_BUF_SIZE];

			strcpy(tmp, *file);

			tmp_string = tmp;
			const char* extension = tmp_string.erase(0, tmp_string.find_last_of(".")).c_str();


			if (strcmp(extension, ".meta") != 0 && strcmp(extension, ".json") != 0)
			{
				strcpy(tmp, directory);
				strcat(tmp, *file);
				strcat(tmp, ".meta");

				if (!PHYSFS_exists(tmp))
				{
					strcpy(new_file, directory);
					strcat(new_file, *file);
					ret = true;
				}

			}

			RELEASE_ARRAY(tmp);
		}

		if (ret)
			break;

	}


	PHYSFS_freeList(files);

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

void ModuleFileSystem::RemoveAllDependencies(char* file_path) {


	FILE_TYPE type = FindTypeFile((const char*)file_path);
	std::string file_path_s = file_path;

	if (type == MESH_FILE) {

		
		file_path_s.erase(file_path_s.find_last_of("."), file_path_s.size());
		file_path_s +=".json";

		//charge json -> get his dependencies (.felina) -> delete it
		
		JSON_Value* file_root = json_parse_file(file_path_s.data());
		
		JSON_Array* go_array = json_value_get_array(file_root);
		
		int w = json_array_get_count(go_array);

		for (uint i = 0; i < json_array_get_count(go_array); i++) {
			
			JSON_Object* obj = json_array_get_object(go_array, i);

			JSON_Array* components_array = json_object_get_array(obj, "Components");
			
			for (uint i = 0; i < json_array_get_count(components_array); i++) {


				JSON_Object* obj2 = json_array_get_object(components_array, i);

				if (json_object_get_number(obj2, "type") == 1 || json_object_get_number(obj2, "type") == 2) {
					char* temp = (char*)json_object_get_string(obj2, "path");
					remove((const char*)temp);
				}
			}

		}

		//delete .json
		remove((const char*)file_path_s.c_str());

		file_path_s = file_path;
		file_path_s.append(".meta");
		remove((const char*)file_path_s.c_str());

		//delete .meta
		remove((const char*)file_path_s.c_str());
	}

	else if (type == MATERIAL_FILE) {

		file_path_s.erase(file_path_s.find_last_of("."), file_path_s.size());
		file_path_s.erase(0,file_path_s.find_last_of("/"));
		file_path_s.append(".dds");

		char* library_file;

		sprintf_s(library_file, DEFAULT_BUF_SIZE, "Library/Materials/%s",file_path_s.c_str());

		remove((const char*)file_path_s.c_str());

		file_path_s.append(".meta"); //this meta should be generated in assets

	}
}

void ModuleFileSystem::CreateFolder(const char* new_folder) {

	if (PHYSFS_mkdir(new_folder)) {

		std::string tmp = new_folder;
		tmp.erase(0,tmp.find_last_of("/") + 1);
		PHYSFS_mount(new_folder,tmp.c_str(),1);
	}

}

void ModuleFileSystem::FreeEnumeratedFiles(const char** dir)
{
	PHYSFS_freeList(dir);
}