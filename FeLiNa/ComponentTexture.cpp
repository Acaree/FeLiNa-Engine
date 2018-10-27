#include "ComponentTexture.h"
#include "ModuleTexture.h"



ComponentTexture::ComponentTexture(GameObject* parent) : Component(parent) {

	type = Component_Material;
}

ComponentTexture::~ComponentTexture() {

}


void ComponentTexture::SetTexture(Texture* tex)
{
	this->texture = tex;
}

Texture* ComponentTexture::GetTexture() const
{
	return texture;
}

uint ComponentTexture::GetTextureID() const
{
	if (texture != nullptr)
		return texture->texture_id;
	else
		return 0;
}