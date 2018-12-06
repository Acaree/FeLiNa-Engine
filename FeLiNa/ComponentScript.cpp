#include "ComponentScript.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleInput.h"

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
	if (graph != nullptr)
	{
		graph->Update();
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {

		SaveScript(nullptr);

	}
}


void ComponentScript::DrawInspector()
{
	if (ImGui::TreeNodeEx("Script"))
	{
		if (graph == nullptr)
		{
			ImGui::Text("Don't have any Graph asociated");
			ImGui::Button("Drag Script Here");

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Nodes"))
				{
					std::string payload_n = (char*)payload->Data;
					int position = payload_n.find(".json");

					if (position != std::string::npos)
					{
						//TODO the super resource!
					}
					
				}

				ImGui::EndDragDropTarget();
			}

		}
		else
		{
			ImGui::Text(graph->name);

			if (ImGui::Button("Open Graph"))
			{
				open_graph = true;
			}

			if (open_graph)
				graph->DrawNodeGraph();


		}
		ImGui::TreePop();
	}

}

void ComponentScript::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_number(obj, "UID", uid);
}

void ComponentScript::SaveScript(Node* node) {

	JSON_Value* new_value = json_value_init_array();
	JSON_Array* new_array = json_value_get_array(new_value);


	for (uint i = 0; i < graph->nodes.size(); ++i)
	{
		JSON_Value* node_value = json_value_init_object();
		JSON_Object* node_obj = json_value_get_object(node_value);

		json_object_set_number(node_obj, "type", graph->nodes[i]->type);
		json_object_set_number(node_obj, "subtype", graph->nodes[i]->subtype);
		json_object_set_number(node_obj,"id", graph->nodes[i]->id);
		json_object_set_number(node_obj, "x", graph->nodes[i]->position.x);
		json_object_set_number(node_obj, "y", graph->nodes[i]->position.y);
		json_object_set_number(node_obj, "w", graph->nodes[i]->size.x);
		json_object_set_number(node_obj, "h", graph->nodes[i]->size.y);

		JSON_Value* inputs_value = json_value_init_array();
		JSON_Array* inputs_array = json_value_get_array(inputs_value);
		for (uint j = 0; j < graph->nodes[i]->inputs_vec.size(); j++) {
			JSON_Value* single_value = json_value_init_object();
			JSON_Object* single_obj = json_value_get_object(single_value);
			json_object_set_number(single_obj, "input id", graph->nodes[i]->inputs_vec[j]->id);
			json_array_append_value(inputs_array, single_value);

		}

		json_object_set_value(node_obj, "Inputs", inputs_value);


		JSON_Value* outputs_value = json_value_init_array();
		JSON_Array* outputs_array = json_value_get_array(inputs_value);
		for (uint j = 0; j < graph->nodes[i]->outputs_vec.size(); j++) {
			JSON_Value* single_value = json_value_init_object();
			JSON_Object* single_obj = json_value_get_object(single_value);
			json_object_set_number(single_obj, "output id", graph->nodes[i]->outputs_vec[j]->id);
			json_array_append_value(inputs_array, single_value);

		}

		json_object_set_value(node_obj, "Outputs", inputs_value);

		json_array_append_value(new_array, node_value);
	}

	int size = json_serialization_size_pretty(new_value);
	char* buffer = new char[size];
	json_serialize_to_buffer_pretty(new_value, buffer, size);

	char name[DEFAULT_BUF_SIZE];

	sprintf_s(name, DEFAULT_BUF_SIZE, "Library/test.json");
	

	App->fs->SaveBufferData(buffer, name, size);
	RELEASE_ARRAY(buffer);
	json_value_free(new_value);
}