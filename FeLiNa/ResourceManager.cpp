#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceManager.h"
#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "MeshImporter.h"
#include "MaterialImporter.h"
#include "ModuleTimeManagement.h"
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

update_status ResourceManager::PreUpdate(float dt)
{
	
	/*for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->CountReferences() == 0)
		{
			RELEASE(it->second);
			resources.erase(it);
			
			break;
		}
	}*/

	refresh_time += dt;
	if (refresh_time >= time_to_refresh)
	{
		// FOR SEARCH META: when we implemented, testing with .felina and works pecfect
		/*std::string new_file;
		if (App->fs->FindNewAssetsFiles("Assets/", new_file))// TO Change -> assets a Macro?
			ImportFile(new_file.data());*/

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

	ret = Find(new_file);

	if (ret == 0)
	{
		LOG("Don't find in resources, need load from path...");

		FILE_TYPE file_type = App->fs->FindTypeFile(new_file);

		Resource* resource = nullptr;

		switch (file_type)
		{
		case MESH_FILE:
		{
			LOG("Mesh file detected, creating...");
			resource = CreateNewResource(RESOURCE_TYPE::RESOURCE_MESH);
			resource->SetExportedFile(new_file);
			Mesh* mesh = App->importer_mesh->LoadFLN(new_file);
			memcpy(mesh->felina_path, new_file,DEFAULT_BUF_SIZE);
			SetResourceData(mesh, resource);
			App->importer_mesh->CreateFileMeta(resource);
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
			App->importer_mesh->CreateFileMeta(resource);
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
			ret = resource->GetUID();
		}
		else
			ret = 0;
	}
	else
	{
		LOG("Find file in resources with uid: %i", ret);

		//Get the resource that are create and sum loaded+1
		Resource* resource = Get(ret);
		resource->LoadToMemory();
	}

	return ret;
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