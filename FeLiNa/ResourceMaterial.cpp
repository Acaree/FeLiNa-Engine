#include "ResourceMaterial.h"
#include "MaterialImporter.h"
#include "Glew/include/glew.h"
#include "mmgr/mmgr.h"
ResourceMaterial::ResourceMaterial(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{
	
}

ResourceMaterial::~ResourceMaterial()
{
	//glDeleteTextures(1, (GLuint*) &(id_texture));

}

bool ResourceMaterial::LoadInMemory()
{
	bool ret = true;

	return ret;
}

bool ResourceMaterial::EraseInMemory()
{
	bool ret = true;
	glDeleteTextures(1, (GLuint*) &(id_texture));


	return ret;
}

