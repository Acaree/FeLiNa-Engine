#include "ModuleFileSystem.h"
#include "Application.h"
#include "ResourceManager.h"
#include "MaterialImporter.h"
#include "MeshImporter.h"
#include "Resource.h"
#include "PhysFS/physfs.h"
#include <string>

#include "mmgr/mmgr.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

ModuleFileSystem::ModuleFileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "File System";

	PHYSFS_init(nullptr);
	
	if (PHYSFS_mount(".", nullptr, 1) == 0) {

		LOG("Physfs could not find the path: %s", PHYSFS_getLastError());

	}

	PHYSFS_setWriteDir(".");

	if (PHYSFS_mount("./Assets/", "Assets", 1)==0) { 
	
		LOG("Physfs could not find the path: %s", PHYSFS_getLastError());

	}

	//Get all folder in Assets and mount
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
				LOG("Physfs could not find the path: %s", PHYSFS_getLastError());

		RELEASE(stat);
	}


	PHYSFS_mkdir("Library/Meshes");
	PHYSFS_mkdir("Library/Materials");

	if (PHYSFS_mount("./Library/", "Library", 1) == 0) { 

		LOG("Physfs could not find the path: %s", PHYSFS_getLastError());

	}

	if (PHYSFS_mount("./Library/Meshes/", "Meshes", 1) == 0) {

		LOG("Physfs could not find the path: %s", PHYSFS_getLastError());

	}

	if (PHYSFS_mount("./Library/Materials/", "Materials", 1) == 0) {

		LOG("Physfs could not find the path: %s", PHYSFS_getLastError());

	}

}

ModuleFileSystem::~ModuleFileSystem()
{
	PHYSFS_deinit();
}

update_status ModuleFileSystem::PreUpdate(float dt)
{
	if (refresh_now)
	{
		assets_files.clear();

		std::string path;
		GetAllAssetsFiles("Assets", path);

		if (assets_files.size() > 0)
		{
			CheckAllAssetsFiles();
		}

		refresh_now = false;
	}


	return UPDATE_CONTINUE;
}

void ModuleFileSystem::CheckAllAssetsFiles()
{
	std::string extension;
	std::string file;

	uint null_position = std::string::npos;
	uint position = 0;
	for (std::list<std::string>::const_iterator it = assets_files.begin(); it != assets_files.end(); ++it)
	{
		extension = *it;
		position = extension.find_last_of(".");

		if (position != null_position)
		{
			extension.erase(0, position + 1);

			if (strcmp(extension.c_str(), "meta") == 0)
			{
				file = *it;
				position = file.find_last_of(".");

				if (position != null_position)
				{
					file.erase(position, file.size());

					if (ExistFile(file.c_str()))
					{
						uint mod_time_file = GetLastModificationTime(file.c_str());
						uint meta_mod_time = App->importer_mesh->GetLastModificationTime((*it).c_str());

						if (mod_time_file != meta_mod_time)
						{
							//the original file has overwritten, delete resource and dependency and reimport.
							FILE_TYPE type = FindTypeFile(file.c_str());

							App->resource_manager->CreateNewResource(type,file.c_str(),(*it).c_str());
							
						}

					}
					else
					{
						//not exist original fbx delete all.
						RemoveAllDependencies((char*)file.c_str());
					}

				}
			}
			else
			{
				//if not are a meta we search if exist meta file, if not exist create
				file = *it;
				file += ".meta";

				if (!ExistFile(file.c_str()))
				{
					//Create meta and import all.
					App->resource_manager->ImportFile((*it).c_str());
				}

			}

		}

	}
}

void ModuleFileSystem::GetAllAssetsFiles(const char* dir, std::string path)
{
	path.append(dir);
	path.append("/");

	const char** files = App->fs->GetAllFilesFromDir(dir);
	const char** file;

	for (file = files; *file != nullptr; ++file)
	{
		//Recursive for all folders
		if (App->fs->isDirectory(*file))
		{
			GetAllAssetsFiles(*file, path);

			//test equivalent to not found
			uint position = path.find(*file);

			if (position != std::string::npos)
				path = path.substr(position, path.size());
		}
		else
		{
			std::string extension = *file;
			extension.erase(0, extension.find_last_of(".") + 1);

			if (strcmp(extension.c_str(), "json") == 0)
				continue;

			extension = path;
			extension += *file;

			assets_files.push_back(extension.data());

		}
	}

}


void ModuleFileSystem::GetPhysfsStats(const char* file, PHYSFS_Stat & stats)
{
	PHYSFS_stat(file, &stats);
}

uint ModuleFileSystem::Load(const char* filePath, char** buffer) const {
	
	uint count = 0;

	bool file_exists = PHYSFS_exists(filePath);

	if (file_exists) {
		
		PHYSFS_file* file = PHYSFS_openRead(filePath);

		if (file != nullptr) {

			PHYSFS_sint64 size = PHYSFS_fileLength(file);

			if (size > 0) {
				
				*buffer = new char[size];
				count = PHYSFS_readBytes(file, *buffer, size);

				if (count == size)
				{
					LOG("Read %u bytes from file '%s'", count, filePath);
					
				}
				else
				{
					RELEASE_ARRAY(buffer);
					LOG("Could not read file '%s'. ERROR: %s", filePath, PHYSFS_getLastError());
				}

				PHYSFS_close(file);
			}
			else
			{
				LOG("Size Error'. ERROR: %s", filePath, PHYSFS_getLastError());
			}
		}

	}
	else
		LOG("File '%s' don't exists and can't be charged. %s", filePath, PHYSFS_getLastError());

	return count;
}


char* ModuleFileSystem::SaveFile(char* buffer, uint size, std::string& output_file, FILE_TYPE file)
{

	char new_name[DEFAULT_BUF_SIZE];
	sprintf_s(new_name, DEFAULT_BUF_SIZE, output_file.data());
	output_file = new_name;

	char new_path[DEFAULT_BUF_SIZE];

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
	output_file = new_path;

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

	if (!exists) {
		file = PHYSFS_openAppend(file_path);
		LOG("New file will be created : %s ", file_path)
	}
	else {
		file = PHYSFS_openWrite(file_path);
		LOG("File opened to write : %s ", file_path)
	}
	if (file != nullptr)
	{
		count = PHYSFS_writeBytes(file, (const void*)buffer, size);

		if (count == size)
		{
			if (exists)
			{
				LOG("New file created : %s ",file_path);
			}
			else
				LOG("Can't create the new file '%s' : %s ", file_path, PHYSFS_getLastError());
		}
		
		PHYSFS_close(file);
	}

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

	return file_type;
}

bool ModuleFileSystem::ExistFile(const char* file)
{
	return PHYSFS_exists(file);
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

		
		JSON_Value* file_root = json_parse_file(file_path_s.data());
		
		JSON_Array* go_array = json_value_get_array(file_root);
		
		int w = json_array_get_count(go_array);

		for (uint i = 0; i < json_array_get_count(go_array); i++) {
			
			JSON_Object* obj = json_array_get_object(go_array, i);

			JSON_Array* components_array = json_object_get_array(obj, "Components");
			
			for (uint i = 0; i < json_array_get_count(components_array); i++) {


				JSON_Object* obj2 = json_array_get_object(components_array, i);

				if (json_object_get_number(obj2, "type") == 1){
					int uid = json_object_get_number(obj2, "UID");

					Resource* resource = App->resource_manager->Get(uid);
					resource->SetInvalidateResource();

					char* temp = new char[DEFAULT_BUF_SIZE];
					sprintf(temp, "Library/Meshes/%i.felina", uid);
					FileDelete((const char*)temp);
					
					RELEASE_ARRAY(temp);
				}

				else if (json_object_get_number(obj2, "type") == 2) {
					int uid = json_object_get_number(obj2, "UID");

					Resource* resource = App->resource_manager->Get(uid);
					resource->SetInvalidateResource();

					char* temp = new char[DEFAULT_BUF_SIZE];
					sprintf(temp, "Library/Materials/%i.dds", uid);
					FileDelete((const char*)temp);
					RELEASE_ARRAY(temp);
				}
				}

		}

		//delete .json
		FileDelete(file_path_s.c_str());

		file_path_s = file_path;
		file_path_s.append(".meta");
		FileDelete(file_path_s.c_str());

		//delete .meta
		FileDelete(file_path_s.c_str());
	}

	else if (type == MATERIAL_FILE) {

		FileDelete(file_path_s.c_str());
		file_path_s += ".meta";

		int uid = App->importer_material->GetUIDofMeta(file_path_s.c_str());
		char* temp = new char[DEFAULT_BUF_SIZE];
		sprintf(temp, "Library/Materials/%i.dds", uid);

		FileDelete(temp);

		RELEASE_ARRAY(temp);

	}
}

void ModuleFileSystem::CreateFolder(const char* new_folder) {

	if (PHYSFS_mkdir(new_folder)) {

		std::string tmp = new_folder;
		tmp.erase(0,tmp.find_last_of("/") + 1);
		PHYSFS_mount(new_folder,tmp.c_str(),1);
		LOG("New folder %s created and ready to use", new_folder);
	}

}

void ModuleFileSystem::FreeEnumeratedFiles(const char** dir)
{
	PHYSFS_freeList(dir);
}

bool ModuleFileSystem::RecursiveFindFileExistInAssets(const char* dir, const char* file_name, std::string& path)
{
	bool ret = false;

	if (dir != nullptr)
	{
		path.append(dir);
		path += "/";

		std::string file = path;
		file += file_name;

		if (ExistFile(file.c_str()))
		{
			path += file_name;
			ret = true;
		}
		else
		{
			const char** files = GetAllFilesFromDir(dir);
			const char** file;

			for (file = files; *file != nullptr; ++file)
			{
				if (isDirectory(*file))
				{
					if (RecursiveFindFileExistInAssets(*file, file_name, path))
					{
						return true;
					}

					uint position = path.find(*file);

					if (position != std::string::npos)
						path.erase(position, path.size());

				}
			}
		}
	}

	return ret;
}

void ModuleFileSystem::DeleteFolderandContainedFiles(const char* folder_to_remove) {

	const char** directory_array = App->fs->GetAllFilesFromDir(folder_to_remove);
	std::string file_path;

	for (const char** file = directory_array; *file != nullptr; ++file) {

		file_path = folder_to_remove;
		file_path += *file;
		App->fs->RemoveAllDependencies((char*)file_path.c_str());
		FileDelete(file_path.c_str());
	}

	file_path.clear();
	FileDelete(folder_to_remove);
}

uint ModuleFileSystem::GetLastModificationTime(const char* dir)
{
	return PHYSFS_getLastModTime(dir);
}

void ModuleFileSystem::FileDelete(const char* path)
{
	if (PHYSFS_delete(path))
	{
		LOG("FILE DELETED");
	}
	else
		LOG("ERROR: FILE NOT DELETED");
}