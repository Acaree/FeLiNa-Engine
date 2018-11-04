#include "ComponentTexture.h"
#include "ImGui/imgui.h"


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

uint ComponentTexture::GetTextureHeight() const {

	return texture->height;
}

uint ComponentTexture::GetTextureWidth() const {

	return texture->width;

}

void ComponentTexture::DrawInspector()
{

	if (ImGui::TreeNodeEx("Material"))
	{
		ImGui::Text("Texture ID: %i", texture->texture_id);
		ImGui::Separator();

		ImGui::Checkbox("Checker Material", &material_checker);
		ImGui::Checkbox("No Texture", &no_texture);

		ImGui::Text("Width: %i", texture->width);
		ImGui::SameLine();
		ImGui::Text("Height: %i", texture->height);

		ImGui::Image((ImTextureID)(texture->texture_id), ImVec2(250, 250));


		ImGui::TreePop();
	}




}