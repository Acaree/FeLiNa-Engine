#include "ResourceMaterial.h"
#include "MaterialImporter.h"
#include "Glew/include/glew.h"

ResourceMaterial::ResourceMaterial(uint uid, RESOURCE_TYPE type) : Resource(uid, type)
{
	texture = new Texture();
}

ResourceMaterial::~ResourceMaterial()
{
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