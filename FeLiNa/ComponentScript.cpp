#include "ComponentScript.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Resource.h"
#include "ModuleResourceManager.h"
#include "ResourceScript.h"

#include "NodeGraph.h"
#include "NodeInputKeyboard.h"
#include "NodeTranslateGameObject.h"
#include "NodeMouseMotion.h"
#include "NodeRotateGameObject.h"
#include "NodeInputMouse.h"
#include "NodeInstatiateGameObject.h"

ComponentScript::ComponentScript(GameObject* parent) : Component(parent)
{
	type = Component_Script;
	graph = new NodeGraph();
}

ComponentScript::~ComponentScript()
{
	RELEASE(graph);
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
						payload_n.erase(payload_n.find(EXTENSION_SCRIPT)+ strlen(EXTENSION_SCRIPT), payload_n.size());
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


		ImGui::Text(graph->name);

		if (ImGui::Button("Open Graph"))
		{
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

	JSON_Value* new_value = json_value_init_array();
	JSON_Array* new_array = json_value_get_array(new_value);

	for (uint i = 0; i < graph->nodes.size(); ++i)
	{
		JSON_Value* node_value = json_value_init_object();
		JSON_Object* node_obj = json_value_get_object(node_value);

		
		graph->nodes[i]->SetNodeReferencesInJSON(node_obj);

		json_array_append_value(new_array, node_value);
	}

	json_object_set_value(obj,"References", new_value);
}

