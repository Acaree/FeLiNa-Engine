#include "ComponentTexture.h"
#include "ComponentMesh.h"



ComponentTexture::ComponentTexture(GameObject* parent, uint texture_id) : Component(parent) {

	this->texture_id = texture_id;

}

ComponentTexture::~ComponentTexture() {

}


void ComponentTexture::ApplyTextureToMesh() {

	

}