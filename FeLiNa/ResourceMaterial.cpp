#include "ResourceMaterial.h"
#include "MaterialImporter.h"
#include "Glew/include/glew.h"
#include "mmgr/mmgr.h"
ResourceMaterial::ResourceMaterial(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{
	texture = new Texture;
}

ResourceMaterial::~ResourceMaterial()
{
	glDeleteTextures(1, (GLuint*) &(texture->texture_id));
	//RELEASE_ARRAY(texture->felina_path);
	//RELEASE(texture);
}

bool ResourceMaterial::LoadInMemory()
{
	bool ret = true;

	return ret;
}

bool ResourceMaterial::EraseInMemory()
{
	bool ret = true;
	glDeleteTextures(1, (GLuint*) &(texture->texture_id));

	RELEASE(texture);
	return ret;
}

void ResourceMaterial::SetTexture(Texture* texture)
{
	this->texture = texture;
}

Texture* ResourceMaterial::GetTexture() const
{
	return texture;
}