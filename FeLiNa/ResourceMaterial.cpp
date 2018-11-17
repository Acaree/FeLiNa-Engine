#include "Glew/include/glew.h"
#include "ResourceMaterial.h"
#include "MaterialImporter.h"
#include "Application.h"
#include "mmgr/mmgr.h"
ResourceMaterial::ResourceMaterial(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{
	
}

ResourceMaterial::~ResourceMaterial()
{
}

bool ResourceMaterial::LoadInMemory()
{
	bool ret = true;
	
	std::string meta = exported_file.c_str();
	meta += ".meta";
	MaterialSettings* settings = new MaterialSettings();

	App->importer_material->GetImportSettingsInMeta(meta.c_str(),settings);
	App->importer_material->Load(file.c_str(),this,settings);

	RELEASE(settings);
	return ret;
}

bool ResourceMaterial::EraseInMemory()
{
	bool ret = true;
	glDeleteTextures(1, (GLuint*) &(id_texture));


	return ret;
}

