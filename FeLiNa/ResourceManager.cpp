#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"

#include "Importer.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"

#include "ModuleTimeManagement.h"
#include "SceneSerialization.h"
#include "PhysFS/physfs.h"
#include "mmgr/mmgr.h"
ResourceManager::ResourceManager(Application* app, bool start_enabled ): Module(app,start_enabled)
{

}

ResourceManager::~ResourceManager()
{
	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
		RELEASE(it->second);

	resources.clear();
}

bool ResourceManager::Start()
{
	//Create all resources emptys
	std::string path;
	RecursiveResourceFiles("Assets", path);



	return true;
}

update_status ResourceManager::PreUpdate(float dt)
{


	return UPDATE_CONTINUE;
}


//The file need to load in ModuleFileSystem
uint ResourceManager::Find(const char* file) const
{
	
	for (std::map<uint, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
		if (strcmp(it->second->GetExportedFile(), file) == 0)
			return it->first;

	return 0;
}

uint ResourceManager::ImportFile(const char* assets_file, const char* meta_file, const char* library_file)
{
	uint ret = 0;

	RESOURCE_TYPE resource_type = RESOURCE_TYPE::RESOURCE_DEFAULT;

	if (assets_file != nullptr)
	{
		//Find the type of file from it's extension
		bool is_imported = false;

		FILE_TYPE file_type = App->fs->FindTypeFile(assets_file);
		ImporterSettings* settings = nullptr;
		std::string output_file;
		std::list<uint> uids;

		switch (file_type)
		{
		case MESH_FILE:
			settings = new MeshSettings();
			resource_type = RESOURCE_TYPE::RESOURCE_MESH;
			break;
		case MATERIAL_FILE:
			settings = new MaterialSettings();
			resource_type = RESOURCE_TYPE::RESOURCE_MATERIAL;
			break;

		}

		if (library_file != nullptr)
		{
			is_imported = true;
			output_file = library_file;
		}
		else
		{

			if (meta_file != nullptr)
			{
				switch (file_type)
				{
				case MESH_FILE:
					App->importer_mesh->ReadFileMeta(meta_file, (MeshSettings*)settings);
					break;
				case MATERIAL_FILE:
					App->importer_material->GetImportSettingsInMeta(meta_file, (MaterialSettings*)settings);
					break;

				}
			}
			//Get the name and the path
			std::string name = assets_file;
			std::string path = assets_file;

			path.erase(name.find_last_of("/") +1, name.size() );
			name.erase(0, name.find_last_of("/")+1);

		

			switch (file_type)
			{
			case MESH_FILE:
				is_imported = App->importer_mesh->Import(name.c_str(), path.c_str(),output_file, (MeshSettings*)settings);
				resource_type = RESOURCE_TYPE::RESOURCE_MESH;
				break;
			case MATERIAL_FILE:
				is_imported= App->importer_material->Import(assets_file, output_file,(MaterialSettings*)settings);
				resource_type = RESOURCE_TYPE::RESOURCE_MATERIAL;
				break;
			}
		}

		//If importation go good we finally create the resources.
		if (is_imported)
		{
			std::list<Resource*> aux_resources;

			switch (resource_type)
			{
			case RESOURCE_MESH:
			{
				std::string json_file = assets_file;
				json_file.erase(json_file.find_last_of("."), json_file.size());
				json_file += ".json";

				App->serialization_scene->GetAllUIDInSerialization(uids, json_file.c_str(), file_type);

				
				//Create all resources empty
				for (std::list<uint>::const_iterator it = uids.begin(); it != uids.end(); ++it)
				{
					Resource* resource = CreateNewResource(resource_type, *it);
					resource->exported_file = "Library/";
					resource->exported_file += "Meshes/";
					resource->exported_file += std::to_string(*it);
					resource->exported_file += ".felina";
					resource->file = assets_file;
					aux_resources.push_back(resource);
				}

				if (aux_resources.size() != 0)
				{
					FillResources(aux_resources, settings);
					ret = aux_resources.front()->GetUID();
				}
				App->importer_mesh->CreateFileMeta(aux_resources, (MeshSettings*)settings);

				break;
			}
			case RESOURCE_MATERIAL:
			{
				std::string output_name = output_file;
				output_name.erase(0, output_name.find_last_of("/")+1);
				output_name.erase(output_name.find_last_of("."), output_file.size());
				uint uid = strtoul(output_name.c_str(), NULL, 0);
				Resource* resource = CreateNewResource(resource_type, uid);
				resource->file= output_file;
				resource->exported_file = assets_file;
				aux_resources.push_back(resource);

				
				App->importer_material->CreateFileMeta(aux_resources, (MaterialSettings*)settings);

				//FillResources(aux_resources, settings);
				ret = aux_resources.front()->GetUID();
				break;
			}

			}


		}

		
		RELEASE(settings);
	}

	return ret;
}

void ResourceManager::FillResources(std::list<Resource*> resources, ImporterSettings* settings)
{

	for (std::list<Resource*>::iterator it = resources.begin(); it != resources.end(); it++)
	{
		FILE_TYPE type = App->fs->FindTypeFile((*it)->file.c_str());
		
		switch (type)
		{
		case MESH_FILE:
			App->importer_mesh->LoadFLN((*it)->exported_file.c_str(),(ResourceMesh*)(*it));
			break;
		/*case MATERIAL_FILE:
			App->importer_material->Load((*it)->file.c_str(), (ResourceMaterial*)(*it), (MaterialSettings*)settings); TO REVISE
			break;*/

		}
	}
}

void ResourceManager::RecursiveResourceFiles(const char* dir, std::string path)
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
			RecursiveResourceFiles(*file, path);

			//test equivalent to not found
			uint position = path.find(*file);

			if (position != std::string::npos)
				path = path.substr(position, path.size());
		}
		else
		{
			std::string extension = *file;
			extension.erase(0, extension.find_last_of(".")+1);

			if (strcmp(extension.c_str(), "meta") == 0 || strcmp(extension.c_str(), "json") == 0)
				continue;

			std::string new_file = path;
			std::string meta_file = path;

			meta_file += *file;
			meta_file += ".meta";

			if (!App->fs->ExistFile(meta_file.c_str()))
			{
				new_file.append(*file);
				ImportFile(new_file.c_str());

			}
			else
			{
				//Need take all UID that save in meta files.
				FILE_TYPE type = App->fs->FindTypeFile(*file);
				std::list<uint> uids;

				switch (type)
				{
				case FILE_TYPE::MESH_FILE:
					App->serialization_scene->GetAllUIDMeshesInMeta(uids, meta_file.c_str(), FILE_TYPE::MESH_FILE);
					break;
				case FILE_TYPE::MATERIAL_FILE:
					App->serialization_scene->GetAllUIDMeshesInMeta(uids, meta_file.c_str(), FILE_TYPE::MATERIAL_FILE);
					break;
				}

				std::string library_file;

				bool in_directory = true;

				for (std::list<uint>::const_iterator it = uids.begin(); it != uids.end(); ++it)
				{

					switch (type)
					{
					case FILE_TYPE::MESH_FILE:
						library_file ="Library/Meshes/";
						library_file += std::to_string(*it);
						library_file += ".felina";
						break;
					case FILE_TYPE::MATERIAL_FILE:
						library_file = "Library/Materials/";
						library_file += std::to_string(*it);
						library_file += ".dds";
						break;
					}


					if (!App->fs->ExistFile(library_file.c_str()))
					{
						in_directory = false;
						break;
					}

				}
				//WE need check this
				if (in_directory)
				{
					/*library_file = *file;
					library_file.erase(0, library_file.find_last_of("\\") + 1);
					library_file.erase(library_file.find_last_of("."), library_file.size());*/

					new_file.append(*file);

					if (Find(new_file.c_str())==0)
					{
						//In this case its all but resources not created 
						//ImportFile() Need to change the Import to char file and meta and exported
						ImportFile(new_file.c_str(),meta_file.c_str(),library_file.c_str());
					}

				}
				else
				{
					//In this case we have file and meta but don't have the file in library
					new_file.append(*file);
					//ImportFile()
				}
			}
		}
	}
}


 Resource* ResourceManager::Get(uint uid)
{
	Resource* res = nullptr;

	std::map<uint, Resource*>::iterator it= resources.find(uid);

	if (it != resources.end())
		res = it->second;

	return res;
}

Resource* ResourceManager::CreateNewResource(RESOURCE_TYPE type, uint last_uid)
{
	Resource* resource = nullptr;

	if (type != RESOURCE_TYPE::RESOURCE_DEFAULT)
	{
		uint uid;

		if (last_uid == 0)
			uid = App->random->Int();
		else
			uid = last_uid;

		switch (type)
		{
		case RESOURCE_MESH:
			resource = (Resource*) new ResourceMesh(uid, RESOURCE_MESH);
			break;
		case RESOURCE_MATERIAL:
			resource = (Resource*) new ResourceMaterial(uid, RESOURCE_MATERIAL);
			break;
		default:
			break;
		}

		if (resource != nullptr)
		{
			LOG("NEW RESOURCE GENERATED");
			resources[uid] = resource;
		}
		else
			LOG("Can't generate new resource :(");
	}

	return resource;
}

void ResourceManager::CreateNewResource(FILE_TYPE type, const char* asset_file, const char* meta_file)
{
	std::list<uint> uids;
	App->serialization_scene->GetAllUIDMeshesInMeta(uids, asset_file, type);

	std::string library_file;
	switch (type)
	{
	case MESH_FILE:

		for (std::list<uint>::const_iterator it = uids.begin(); it != uids.end(); ++it)
		{
			Resource* resource = Get(*it);
			resource->SetInvalidateResource();

			library_file = "Library/";
			library_file += "Meshes/";
			library_file += std::to_string(*it);
			library_file += ".felina";

			App->fs->FileDelete(library_file.c_str());

			

		}

		break;
	case MATERIAL_FILE:

		for (std::list<uint>::const_iterator it = uids.begin(); it != uids.end(); ++it)
		{
			Resource* resource = Get(*it);
			resource->SetInvalidateResource();

			
			library_file = "Library/";
			library_file += "Materials/";
			library_file += std::to_string(*it);
			library_file += ".dds";

			App->fs->FileDelete(library_file.c_str());
		}

		break;
	}

	std::map<uint, Resource*>::iterator tmp_resource;

	for (std::list<uint>::const_iterator it = uids.begin(); it != uids.end(); ++it)
	{
		tmp_resource = resources.find(*it);

		if (tmp_resource != resources.end())
		{
			RELEASE(tmp_resource->second);
			resources.erase(*it);
		}

	}

	ImportFile(asset_file,meta_file);


}

