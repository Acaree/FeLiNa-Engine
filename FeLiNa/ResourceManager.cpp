#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "ModuleTimeManagement.h"
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
	std::string tmp_string = " ";
	//to revise
	while (!App->fs->isDirectory(tmp_string.c_str()))
	{
		tmp_string.clear();
		char* new_file = new char[DEFAULT_BUF_SIZE];
		if (App->fs->FindNewAssetsFiles("Assets/", new_file))// TO REVISE
		{
			tmp_string = new_file;
			uint uid = ImportFile(new_file);
			Resource* resource = App->resource_manager->Get(uid);

			std::string path = new_file;
			path = path.substr(0, path.find_last_of("/") + 1);

			std::string file_name = new_file;
			file_name = file_name.substr(file_name.find_last_of("/") + 1, file_name.size());

			std::string output_file;

			switch (resource->type)
			{
			case RESOURCE_TYPE::RESOURCE_MESH:
			{
				MeshSettings* default_settings = new MeshSettings();
				App->importer_mesh->Import(file_name.c_str(), path.c_str(), output_file, default_settings);
				RELEASE(default_settings);
			}
			break;
			case RESOURCE_TYPE::RESOURCE_MATERIAL:
				MaterialSettings * default_settings = new MaterialSettings();
				App->importer_material->Import(new_file, output_file, default_settings);
				break;
			}
		}
		RELEASE_ARRAY(new_file);
	}
	return true;
}

update_status ResourceManager::PreUpdate(float dt)
{
	refresh_time += dt;
	if (refresh_time >= time_to_refresh )
	{
		// FOR SEARCH META: when we implemented, testing with .felina and works pecfect
		char* new_file = new char[DEFAULT_BUF_SIZE];
		if (App->fs->FindNewAssetsFiles("Assets/", new_file))// TO Change -> assets a Macro?
			ImportFile(new_file);

		RELEASE_ARRAY(new_file);
		refresh_time = 0.0F;
	}

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

uint ResourceManager::ImportFile(const char* new_file)
{
	uint ret = 0;

	LOG("Importing file: %s", new_file);

	//Find the type of file from it's extension
	FILE_TYPE file_type = App->fs->FindTypeFile(new_file);

	//Generate path .meta 
	std::string meta_file = new_file;
	meta_file += ".meta";

	//Search if exist meta: are a new file
	if (!PHYSFS_exists(meta_file.c_str()))
	{
		//if not exist and not exist resource create all:
		Resource* resource;
		ImporterSettings* settings = nullptr;

		//Create Resource only with path and generate standar .meta.
		switch (file_type)
		{
		case MESH_FILE:
		{
			LOG("Mesh file detected, creating...");
			settings = new MeshSettings(); //Mesh Settings are public ImporterSettings
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);
			resource->SetExportedFile(new_file);
			App->importer_mesh->CreateFileMeta(resource,(MeshSettings*)settings); //TO REVISE: we save options as bool, we don't know if work
			break;
		}
		case MATERIAL_FILE:
		{
			LOG("Material file detected, creating...");
			settings = new MaterialSettings(); //Material Settings are public ImporterSettings
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MATERIAL);
			resource->SetExportedFile(new_file);
			App->importer_material->CreateFileMeta(resource, (MaterialSettings*)settings);
			break;
		}
		case UKNOWN_FILE:
			LOG("Can't recognize type of file");
			break;
		}

		RELEASE(settings);
	}
	else
	{
		
		//if the new file has .meta associate read the meta and add in settings 
		switch (file_type)
		{
		case MESH_FILE:
		{			
			MeshSettings* settings = new MeshSettings();
			App->importer_mesh->ReadFileMeta(meta_file.data(), (MeshSettings*)settings);
			RELEASE(settings);
			break;
		}
		case MATERIAL_FILE:
		{
			MaterialSettings* settings = new MaterialSettings();
			App->importer_material->ReadFileMeta(meta_file.data(), (MaterialSettings*)settings);
			RELEASE(settings);
			break;
		}
		default:
			break;
		}

	}
	ret = Find(new_file);
	return ret;
}

uint ResourceManager::ImportOwnFile(const char* new_file) {
	
	uint uid = Find(new_file);

	if (uid == 0) {
		Resource* resource = nullptr;
		FILE_TYPE file_type = App->fs->FindOwnTypeFile(new_file);

		switch (file_type)
		{
		case MESH_FILE:
		{
			LOG("Mesh file detected, creating...");
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);
			resource->SetExportedFile(new_file);
			Mesh* mesh = App->importer_mesh->LoadFLN(new_file);
			memcpy(mesh->felina_path, new_file, DEFAULT_BUF_SIZE);
			SetResourceData(mesh, resource);
			
			break;
		}
		case MATERIAL_FILE:
		{
			LOG("Material file detected, creating...");
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MATERIAL);
			resource->SetExportedFile(new_file);
			Texture* texture = App->importer_material->LoadDDS((char*)new_file); // To revise function not const char*??
			memcpy(texture->felina_path, new_file, DEFAULT_BUF_SIZE);
			SetResourceData(texture, resource);
			
			break;
		}
		case UKNOWN_FILE:
			LOG("Can't recognize type of file");
			break;
		}

		if (resource != nullptr)
		{
			//If the resource create and assign her data , call LoadToMemory for bind data or sum the number of loaded
			resource->LoadToMemory();
			//Get the uid 
			uid = resource->GetUID();
		}
		else
			uid = 0;
	}
	else
	{
		LOG("Find file in resources with uid: %i", uid);

		//Get the resource that are create and sum loaded+1
		Resource* resource = Get(uid);
		resource->LoadToMemory();
	}

	return uid;
}

 Resource* ResourceManager::Get(uint uid)
{
	Resource* res = nullptr;

	std::map<uint, Resource*>::iterator it= resources.find(uid);

	if (it != resources.end())
		res = it->second;

	return res;
}

Resource* ResourceManager::CreateNewResource(RESOURCE_TYPE type)
{
	Resource* resource = nullptr;

	if (type != RESOURCE_TYPE::RESOURCE_DEFAULT)
	{
		uint uid = App->random->Int();

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