#include "ComponentScript.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Resource.h"
#include "ModuleResourceManager.h"
#include "Application.h"
#include "ModuleGui.h"
#include "ModuleScene.h"
#include "ResourceScript.h"


ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = Component_Script;
}

ComponentScript::~ComponentScript()
{

}

void ComponentScript::Update(float dt)
{
	
	if (uid != 0)
	{
		ResourceScript* resource = (ResourceScript*)App->resource_manager->Get(uid);

		resource->graph->Update();

		if (open_graph)
		{

			resource->graph->DrawNodeGraph();
			
			if(ImGui::IsMouseClicked(1) && !ImGui::IsMouseHoveringAnyWindow())
				open_graph = false;
		}
	}

}


void ComponentScript::DrawInspector()
{
	if (ImGui::TreeNodeEx("Script"))
	{

		ImGui::Button("Drag Script Here");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Nodes"))
			{
				std::string payload_n = (char*)payload->Data;
				int position = payload_n.find(EXTENSION_SCRIPT);

				if (position != std::string::npos)
				{
					payload_n.erase(payload_n.find(EXTENSION_SCRIPT) + strlen(EXTENSION_SCRIPT), payload_n.size());
					uid = App->resource_manager->Find(payload_n.c_str());

					if (uid != 0)
					{
						Resource* resource = App->resource_manager->Get(uid);
						resource->LoadToMemory();
					}
				}

			}

			ImGui::EndDragDropTarget();
		}

		if(uid == 0)
			ImGui::Text("Not script asociated");
		else
		{
			Resource* resource = App->resource_manager->Get(uid);
			ImGui::Text(resource->exported_file.c_str());
			ImGui::Text("Reference counting %i", resource->loaded);
		}

		if (ImGui::Button("Open Graph"))
		{
			uint selected_uid = App->gui->uid_selected_graph;
			
			if (selected_uid != 0)
			{
				Resource* res = App->resource_manager->Get(selected_uid);

				res->EraseToMemory();

				App->gui->uid_selected_graph = 0;
			}
			

			if(uid != 0)
				open_graph = true;
		}

		ImGui::TreePop();
	}

}

void ComponentScript::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_number(obj, "UID", uid);


	if (uid != 0)
	{
		ResourceScript* resource = (ResourceScript*)App->resource_manager->Get(uid);

		resource->graph->SaveReferences(obj);
	}
}

void ComponentScript::OnLoad(JSON_Object* obj)
{
	uid = json_object_get_number(obj, "UID");
	if (uid != 0)
	{
		Resource* resource = App->resource_manager->Get(uid);

		if (resource != nullptr)
		{
			resource->LoadToMemory();

			((ResourceScript*)resource)->graph->LoadReferences(obj);

		}
	}
}