#include "ComponentMaterial.h"
#include "ImGui/imgui.h"
#include "Application.h"
#include "ResourceManager.h"
#include "ResourceMaterial.h"
#include "mmgr/mmgr.h"
ComponentMaterial::ComponentMaterial(GameObject* parent) : Component(parent) {

	type = Component_Material;
}

ComponentMaterial::~ComponentMaterial()
{

	if (uid != 0)
	{
		ResourceMaterial* resource_material = (ResourceMaterial*)App->resource_manager->Get(uid);
		if(resource_material != nullptr)
			resource_material->EraseToMemory();
	}
}


void ComponentMaterial::SetUID(uint uid)
{
	this->uid = uid;
}

uint ComponentMaterial::GetUID() const
{
	return uid;
}



void ComponentMaterial::DrawInspector()
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
			{
				ImGui::Text("Invalid Texture");
			
			}
		}
		else
		{
			ImGui::Text("Invalid Texture");
		
		}

		
			ImGui::Button("Drag Material Here");


			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Nodes"))
				{
					std::string payload_n = (char*)payload->Data;
					payload_n.erase(payload_n.find_first_of("."), payload_n.size());
					Resource* res = App->resource_manager->Get(std::stoi(payload_n.c_str()));

					if (res != nullptr && res->type == RESOURCE_MATERIAL)
					{
						uid = res->uid;
						res->LoadToMemory();
					}

				}
				ImGui::EndDragDropTarget();
			}


		ImGui::TreePop();
	}
}

void ComponentMaterial::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_number(obj, "UID", uid);
}

void ComponentMaterial::OnLoad(JSON_Object* obj)
{
	uid = json_object_get_number(obj, "UID");
	if (uid != 0)
	{
		Resource* resource = App->resource_manager->Get(uid);

		if (resource != nullptr)
			resource->LoadToMemory();
	}
}