#include "ComponentScript.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "ModuleFileSystem.h"
#include "Application.h"
#include "ModuleInput.h"


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
	if (graph != nullptr)
	{
		graph->Update();
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {

		graph->SaveGraph();

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
					int position = payload_n.find(".json");

					//payload_n.substr(payload_n.find_first_of(".") + 5, payload_n.size());

					LoadGraph("Library/test.json");

					if (position != std::string::npos)
					{
						//TODO the super resource!
					}
					
				}

				ImGui::EndDragDropTarget();
			}

		
			ImGui::Text(graph->name);

			if (ImGui::Button("Open Graph"))
			{
				open_graph = true;
			}

			if (open_graph)
				graph->DrawNodeGraph();


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


void ComponentScript::LoadGraph(char* path) {

	JSON_Value* file_root = json_parse_file(path);

	JSON_Array* nodes_array = json_value_get_array(file_root);

	graph = new NodeGraph();

	for (uint i = 0; i < json_array_get_count(nodes_array); ++i) {

		JSON_Object* object = json_array_get_object(nodes_array, i);

		uint subtype_node = json_object_get_number(object, "subtype");

		Node* new_node = nullptr;

		switch (subtype_node) {

		case InputKeyboard: {

			new_node = new NodeInputKeyboard(i);

			break;
		}

		case InputMouse: {

			new_node = new NodeInputMouse(i);

			break;
		}

		case InstatiateGO: {

			new_node = new NodeInstatiateGameObject(i);

			break;
		}

		case MouseMotion: {

			new_node = new NodeMouseMotion(i);

			break;
		}

		case RotateGO: {

			new_node = new NodeRotateGameObject(i);

			break;
		}

		case TranslateGO: {

			new_node = new NodeTranslateGameObject(i);

			break;
		}

		}

		new_node->id = json_object_get_number(object, "id");
		new_node->position.x = json_object_get_number(object, "x");
		new_node->position.y = json_object_get_number(object, "y");
		new_node->size.x = json_object_get_number(object, "w");
		new_node->size.y = json_object_get_number(object, "h");

		graph->nodes.push_back(new_node);

		JSON_Array* inputs = json_object_get_array(object, "Inputs");
		JSON_Array* outputs = json_object_get_array(object, "Outputs");

		for (uint i = 0; i < json_array_get_count(inputs); ++i)
		{
			JSON_Object* input_object = json_array_get_object(inputs, i);

			NodeLink* link = new NodeLink(0,0,0,0);

			link->output_index = json_object_get_number(input_object, "input id");
			link->input_slots = 0;
			link->input_index = new_node->id;
			link->output_slots = 0;

			graph->links.push_back(*link);
		}

		for (uint i = 0; i < json_array_get_count(outputs); ++i)
		{
			JSON_Object* output_object = json_array_get_object(outputs, i);

			NodeLink* link = new NodeLink(0, 0, 0, 0);

			link->input_index = json_object_get_number(output_object, "output id");
			link->input_slots = 0;
			link->output_index = new_node->id;
			link->output_slots = 0;

			graph->links.push_back(*link);
		}
	}

	json_value_free(file_root);

	graph->UpdateNodePointers();


}