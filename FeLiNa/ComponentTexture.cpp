#include "ComponentTexture.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ResourceManager.h"
#include "ResourceMaterial.h"
#include "mmgr/mmgr.h"
ComponentTexture::ComponentTexture(GameObject* parent) : Component(parent) {

	type = Component_Material;
	texture = new Texture();
	texture->felina_path = new char[DEFAULT_BUF_SIZE];
}

ComponentTexture::~ComponentTexture()
{
	uint uid = App->resource_manager->Find(texture->felina_path);

	if (uid != 0)
	{
		ResourceMaterial* resource_mesh = (ResourceMaterial*)App->resource_manager->Get(uid);
		resource_mesh->EraseToMemory();
	}
}

void ComponentTexture::CleanUp()
{
	uint uid = App->resource_manager->Find(texture->felina_path);
	if (uid != 0)
	{
		ResourceMaterial* resource_material = (ResourceMaterial*)App->resource_manager->Get(uid);
		resource_material->EraseToMemory();
	}
	
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
		uint uid = App->resource_manager->Find(texture->felina_path);
		if (uid != 0)
		{
			Resource* resource = App->resource_manager->Get(uid);

			ImGui::Text("Refernce counting: %i", resource->loaded);
		}
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

void ComponentTexture::SetPath(char* path) {

	//texture->felina_path = path;
	memcpy(texture->felina_path, path,DEFAULT_BUF_SIZE);

}

void ComponentTexture::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_string(obj, "path", texture->felina_path);
}

void ComponentTexture::OnLoad(JSON_Object* obj)
{
	char* tmp = (char*)json_object_get_string(obj, "path");
	if (tmp != nullptr)
	{
		uint uid = App->resource_manager->ImportOwnFile(tmp);
		if (uid != 0)
		{
			ResourceMaterial* resource_material = (ResourceMaterial*)App->resource_manager->Get(uid);

			texture = resource_material->GetTexture();
		}
	}

}