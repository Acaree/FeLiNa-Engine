#include "ComponentTexture.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ResourceManager.h"
#include "ResourceMaterial.h"
#include "mmgr/mmgr.h"
ComponentTexture::ComponentTexture(GameObject* parent) : Component(parent) {

	type = Component_Material;
}

ComponentTexture::~ComponentTexture()
{

	if (uid != 0)
	{
		ResourceMaterial* resource_material = (ResourceMaterial*)App->resource_manager->Get(uid);
		if(resource_material != nullptr)
			resource_material->EraseToMemory();
	}
}


void ComponentTexture::SetUID(uint uid)
{
	this->uid = uid;
}

uint ComponentTexture::GetUID() const
{
	return uid;
}



void ComponentTexture::DrawInspector()
{

	if (ImGui::TreeNodeEx("Material"))
	{
		
		if (uid != 0)
		{
			ResourceMaterial* resource = (ResourceMaterial*) App->resource_manager->Get(uid);

			if (resource != nullptr)
			{

				ImGui::Text("Refernce counting: %i", resource->loaded);

				ImGui::Text("Texture ID: %i", resource->id_texture);
				ImGui::Separator();

				ImGui::Text("Width: %i", resource->widht);
				ImGui::SameLine();
				ImGui::Text("Height: %i", resource->height);

				ImGui::Image((ImTextureID)(resource->id_texture), ImVec2(250, 250));
			}
			else
				ImGui::Text("Invalid Texture");
		}
		else
		{
			ImGui::Text("Invalid Texture");
		}
		ImGui::TreePop();
	}
}

void ComponentTexture::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_number(obj, "UID", uid);
}

void ComponentTexture::OnLoad(JSON_Object* obj)
{
	uid = json_object_get_number(obj, "UID");
	if (uid != 0)
	{
		Resource* resource = App->resource_manager->Get(uid);

		if (resource != nullptr)
			resource->LoadToMemory();
	}
}