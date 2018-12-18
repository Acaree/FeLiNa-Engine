#include "Application.h"
#include "ModuleFileSystem.h"
#include "NodeGraph.h"
#include "NodeInputKeyboard.h"
#include "NodeTranslateGameObject.h"
#include "NodeMouseMotion.h"
#include "NodeRotateGameObject.h"
#include "NodeInputMouse.h"
#include "NodeInstatiateGameObject.h"

//NodeGraph-------------------------------------------------------------------------------------------------------------------------------------------
NodeGraph::NodeGraph(uint uid, const char* name)
{

	strcpy(this->name, name);

	if (uid == 0)
		this->uid = App->GenerateRandomNumber();
	else
		this->uid = uid;
}


bool NodeGraph::Update()
{
	bool ret = false;

	for (uint i = 0; i < nodes.size(); ++i)
	{
		if (nodes[i]->type == EventType)
		{
			ret = nodes[i]->Update();
	
			if (ret)
			{
				for (int j = 0; j < nodes[i]->outputs_vec.size(); j++)
				{
					if (!nodes[i]->outputs_vec[j]->Update())
						break;
				}

			}
		}
	}


	return ret;
}


void NodeGraph::SaveGraph()
{
	JSON_Value* root = json_value_init_object();
	JSON_Object* root_object = json_value_get_object(root);

	JSON_Value* new_value = json_value_init_object();
	JSON_Object* new_object = json_value_get_object(new_value);
	json_object_set_value(root_object, "GraphNode", new_value);

	json_object_set_number(new_object, "UID", uid);
	json_object_set_number(new_object, "Size", nodes.size());

	JSON_Value* array_value = json_value_init_array();
	JSON_Array* new_array = json_value_get_array(array_value);

	for (uint i = 0; i < nodes.size(); ++i)
	{
		JSON_Value* node_value = json_value_init_object();
		JSON_Object* node_obj = json_value_get_object(node_value);

		json_object_set_value(new_object, std::to_string(nodes[i]->id).c_str() , node_value);

		nodes[i]->SaveNodeInformation(node_obj);
	}


	uint size = json_serialization_size_pretty(root);
	char* buffer = new char[size];
	json_serialize_to_buffer_pretty(root, buffer, size);


	std::string path = "Assets/";
	path += name;
	path += EXTENSION_SCRIPT;
	App->fs->SaveBufferData(buffer, path.c_str(), size);

	json_value_free(root);

	RELEASE_ARRAY(buffer);

}

void NodeGraph::LoadGraph(const char* path)
{
	JSON_Value* root = nullptr;
	root = json_parse_file(path);

	if (root != nullptr)
	{
		JSON_Object* data = json_value_get_object(root);
		
		if (data != nullptr)
		{
			JSON_Object* graph_object = json_object_get_object(data, "GraphNode");

			if (graph_object != nullptr)
			{
				uint size = json_object_get_number(graph_object, "Size");

				std::string tmp;

				//First create all nodes 
				for (uint i = 0; i < size; ++i)
				{
					tmp = std::to_string(i);
					
					JSON_Object* node_object = json_object_get_object(graph_object, tmp.c_str());

					uint subtype = json_object_get_number(node_object,"Subtype");
					Node* node = CreateNodeByType((NodeSubType)subtype);

					ImVec2 position = { (float)json_object_get_number(node_object, "Px"), (float)json_object_get_number(node_object, "Py") };
					node->position = position;
				}

				//Now create the links 

				int link = -1;


				for (uint i = 0; i < size; ++i)
				{
					tmp = std::to_string(i);
					JSON_Object* node_object = json_object_get_object(graph_object, tmp.c_str());

					JSON_Array* inputs_array = json_object_get_array(node_object, "Inputs");

					for (uint j = 0; j < json_array_get_count(inputs_array); ++j)
					{
						link = json_array_get_number(inputs_array,j);

						nodes[i]->inputs_vec.push_back(nodes[link]);
						links.push_back(NodeGraphicalLink(i,0,link,0));

					}

					JSON_Array* outputs_array = json_object_get_array(node_object, "Outputs");

					for (uint j = 0; j < json_array_get_count(outputs_array); ++j)
					{
						link = json_array_get_number(outputs_array, j);

						nodes[i]->outputs_vec.push_back(nodes[link]);

					}


				}
			}
		}

		json_value_free(root);
	}

}

void NodeGraph::SaveReferences(JSON_Object* obj)
{
	JSON_Value* new_value = json_value_init_array();
	JSON_Array* new_array = json_value_get_array(new_value);

	for (uint i = 0; i < nodes.size();++i)
	{
		JSON_Value* node_value = json_value_init_object();
		JSON_Object* node_obj = json_value_get_object(node_value);

		nodes[i]->SetNodeReferencesInJSON(node_obj);

		json_array_append_value(new_array, node_value);
	}

	json_object_set_value(obj, "References", new_value);
}

void NodeGraph::LoadReferences(JSON_Object* obj)
{
	JSON_Array* arr_references = json_object_get_array(obj, "References");
	
	for (uint i = 0; i < json_array_get_count(arr_references); ++i)
	{
		JSON_Object* new_obj = json_array_get_object(arr_references, i);

		uint id = json_object_get_number(new_obj, "id");

		nodes[id]->GetNodeReferencesInJSON(new_obj);
	}
}

void NodeGraph::DrawNodeGraph()
{
	ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("NodeGraph");

	// LIST OF ALL NODES IN THE LEFT SIZE :/---------------------------------------------------------------------

	static bool open_context_menu = false;
	static int node_hovered_in_list = -1;
	static int node_hovered_in_scene = -1;
	static int node_selected = -1;

	ImGui::BeginChild("All nodes:", ImVec2(100, 0));
	ImGui::Text("All Nodes");
	ImGui::Separator();

	//Iterate all nodes in vector node 
	for (int node_ids = 0; node_ids < nodes.size(); node_ids++) 
	{
		//Push her id and set if are selectable
		Node* node = nodes[node_ids];
		ImGui::PushID(node->id);
		if (ImGui::Selectable(node->name, node->id == node_selected))
			node_selected = node->id;
		if (ImGui::IsItemHovered())
		{
			node_hovered_in_list = node->id;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}
		ImGui::PopID();
	}
	ImGui::EndChild();

	//--------------------------------------------------------------------------------------------------------


	//Create Canvas-------------------------------------------------------------------------------------------

	static bool show_grid = false;

	ImGui::SameLine();
	ImGui::BeginGroup();

	ImGui::Text("Canvas:");
	ImGui::SameLine();

	if (ImGui::Button("Compile"))
	{
		need_save = true;
	}

	ImGui::SameLine(ImGui::GetWindowWidth() - 220);
	ImGui::Checkbox("Show grid", &show_grid);
	ImGui::InputText("Graph Name", name, DEFAULT_BUF_SIZE);

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1, 1));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, IM_COL32(60, 60, 70, 200));
	ImGui::BeginChild("canvas_region", ImVec2(0, 0), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove);
	ImGui::PushItemWidth(120.0f);

	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	static ImVec2 scrolling = ImVec2(0.0f, 0.0f);
	ImVec2 offset = ImGui::GetCursorScreenPos() - scrolling;

	//Show Grid---------------------------------------------------------------------------------------------
	if (show_grid)// Not work
	{
		float grid_size = 64.0f;

		ImVec2 win_pos = ImGui::GetCursorScreenPos();
		ImVec2 canvas_size = ImGui::GetWindowSize();
		for (float x = fmodf(scrolling.x, grid_size); x < canvas_size.x; x += grid_size)
			draw_list->AddLine(ImVec2(x, 0.0f) + win_pos, ImVec2(x, canvas_size.y) + win_pos, GRID_COLOR);
		for (float y = fmodf(scrolling.y, grid_size); y < canvas_size.y; y += grid_size)
			draw_list->AddLine(ImVec2(0.0f, y) + win_pos, ImVec2(canvas_size.x, y) + win_pos, GRID_COLOR);
	}
	//----------------------------------------------------------------------------------------------------

	draw_list->ChannelsSplit(2);
	draw_list->ChannelsSetCurrent(0);

	//Link Nodes------------------------------------------------------------------------------------------ 
	for (int link_idex = 0; link_idex < links.size(); link_idex++)
	{
		NodeGraphicalLink* link = &links[link_idex];
		Node* node_inp = nodes[link->input_index];
		Node* node_out = nodes[link->output_index];
		ImVec2 p1 = offset + node_out->GetOutputSlotPos(link->output_slots);
		ImVec2 p2 = offset + node_inp->GetInputSlotPos(link->input_slots);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);


	}
	//----------------------------------------------------------------------------------------------------


	//Print Nodes--------------------------------------------------------------------------------------------
	for (int node_ids = 0; node_ids < nodes.size(); node_ids++)
	{
		Node* node = nodes[node_ids];
		ImGui::PushID(node->id);

		ImVec2 node_rect_min = offset + node->position;
		draw_list->ChannelsSetCurrent(1);
		bool old_any_active = ImGui::IsAnyItemActive();
		ImGui::SetCursorScreenPos(node_rect_min + NODE_WINDOW_PADDING);

		ImGui::BeginGroup();

		//Here set the the content first that all nodes are equals.
		ImGui::Text("\n");// only aesthetic purpose
		ImGui::Text("%s", node->name);
		
		//Draw node virtual for set variables that we need
		node->DrawNode();
		
		ImGui::EndGroup();

		bool node_widgets_active = (!old_any_active && ImGui::IsAnyItemActive());
		node->size = ImGui::GetItemRectSize() + NODE_WINDOW_PADDING + NODE_WINDOW_PADDING;
		ImVec2 node_rect_max = node_rect_min + node->size;

		//Now we create the node box.

		draw_list->ChannelsSetCurrent(0);

		ImGui::SetCursorScreenPos(node_rect_min);
		ImGui::InvisibleButton("node", node->size);

		if (ImGui::IsItemHovered())
		{
			node_hovered_in_scene = node->id;
			open_context_menu |= ImGui::IsMouseClicked(1);
		}

		bool node_moving_active = ImGui::IsItemActive();
		if (node_widgets_active || node_moving_active)
			node_selected = node->id;
		if (node_moving_active && ImGui::IsMouseDragging(0))
			node->position = node->position + ImGui::GetIO().MouseDelta;

		ImU32 node_bg_color = (node_selected == node->id && node_hovered_in_scene == node->id) ? IM_COL32(75, 75, 75, 255) : IM_COL32(60, 60, 60, 255);
		
		if (node->returned_result)
		{
			draw_list->AddRectFilled(node_rect_min- ImVec2(5,5), node_rect_max + ImVec2(5, 5), IM_COL32(0, 240, 0, 255), 4.0f);
			node->returned_result = false;
		}

		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		
		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);

		for (int slot_idx = 0; slot_idx < node->input_counts; slot_idx++) {
			draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
			ImVec2 mouse_pos = ImGui::GetMousePos();
			ImVec2 input_pos = node->GetInputSlotPos(slot_idx) + offset;
			
			if(ImGui::IsMouseHoveringRect(ImVec2(input_pos.x - NODE_SLOT_RADIUS, input_pos.y - NODE_SLOT_RADIUS), ImVec2(input_pos.x + NODE_SLOT_RADIUS, input_pos.y + NODE_SLOT_RADIUS))) {
				if (ImGui::IsMouseClicked(0)) {
					input_node_pos = node_ids;
					input_clicked = slot_idx;
				}
				if (ImGui::IsMouseClicked(1)) {
					
					for (std::vector<NodeGraphicalLink>::const_iterator it = links.begin(); it != links.end(); it++) {
						
						if ((*it).input_index == node_ids && (*it).input_slots == slot_idx) {
							links.erase(it);
							links.shrink_to_fit();
							it = links.begin();
						}
					}
				}
			}
		}
		for (int slot_idx = 0; slot_idx < node->output_counts; slot_idx++) {
			draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));

			ImVec2 mouse_pos = ImGui::GetMousePos();
			ImVec2 output_pos = node->GetOutputSlotPos(slot_idx) + offset;

			if (ImGui::IsMouseHoveringRect(ImVec2(output_pos.x - NODE_SLOT_RADIUS, output_pos.y - NODE_SLOT_RADIUS), ImVec2(output_pos.x + NODE_SLOT_RADIUS, output_pos.y + NODE_SLOT_RADIUS))) {
				if (ImGui::IsMouseClicked(0)) {
					output_node_pos = node_ids;
					output_clicked = slot_idx;
				}

				if (ImGui::IsMouseClicked(1)) {

					for (std::vector<NodeGraphicalLink>::const_iterator it = links.begin(); it != links.end(); it++) {

						if ((*it).output_index == node_ids && (*it).output_slots == slot_idx) {

							
							std::vector<Node*>::iterator n = nodes[(*it).input_index]->inputs_vec.begin();
							nodes[(*it).input_index]->inputs_vec.erase(n + (*it).input_index-1);
							nodes[(*it).input_index]->inputs_vec.shrink_to_fit();

							//Not need erase in nodes outputs because don't save that
							n = nodes[(*it).output_index]->outputs_vec.begin();
							nodes[(*it).output_index]->outputs_vec.erase(n + (*it).output_index-1);

							links.erase(it);
							links.shrink_to_fit();
							it = links.begin();
							break;
						}
					}
				}
			}
		}

		if (input_node_pos != -1 && output_node_pos != -1) {
			
			Node* input_node = nullptr;
			Node* output_node = nullptr;

			for (uint j = 0; j < nodes.size(); ++j)
			{
		
				if (nodes[j]->id == input_node_pos)
				{
					input_node = nodes[j];
				}

				if (nodes[j]->id == output_node_pos)
				{
					output_node = nodes[j];
				}

			}

			if (input_node != nullptr && output_node != nullptr)
			{
				input_node->inputs_vec.push_back(output_node);
				output_node->outputs_vec.push_back(input_node);
			}

			links.push_back(NodeGraphicalLink(input_node_pos, input_clicked, output_node_pos, output_clicked));
			//nodes[input_node_pos]->outputs_vec.push_back(nodes[output_node_pos]);
			//nodes[output_node_pos]->inputs_vec.push_back(nodes[input_node_pos]);



			input_node_pos = -1;
			output_node_pos = -1;
		}
		

		SetBackgroundNodeType(node, draw_list, node_rect_min, node_rect_max);

		ImGui::PopID();
	}
	
	draw_list->ChannelsMerge();
	//-------------------------------------------------------------------------------------------------------

	//Create New Node----------------------------------------------------------------------------------------

	if (!ImGui::IsAnyItemHovered() && ImGui::IsMouseHoveringWindow() && ImGui::IsMouseClicked(1))
	{
		node_selected = node_hovered_in_list = node_hovered_in_scene = -1;
		open_context_menu = true;
	}
	if (open_context_menu)
	{
		if (ImGui::IsMouseClicked(1))
		{
			ImGui::OpenPopup("context_menu");
			if (node_hovered_in_list != -1)
				node_selected = node_hovered_in_list;
			if (node_hovered_in_scene != -1)
				node_selected = node_hovered_in_scene;
		}
	}


	static bool open_pop = false;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
	if (ImGui::BeginPopup("context_menu"))
	{
		Node* node = node_selected != -1 ? nodes[node_selected] : nullptr;
		ImVec2 scene_pos = ImGui::GetMousePosOnOpeningCurrentPopup() - offset;
		if (node)
		{
			ImGui::Text("Node '%s'", node->name);
			ImGui::Separator();
			if (ImGui::MenuItem("Delete", NULL, false, true))
			{
				DeleteNode(*node);
			}
			
		}
		else
		{
			if (ImGui::MenuItem("Add")) { 
				ImGui::OpenPopup("node_creation");
				open_pop = true;
			}
			if (ImGui::MenuItem("Paste", NULL, false, false)) {}
		}

		ImGui::EndPopup();
		
	}

	if (open_pop)
	{
		ImGui::OpenPopup("node_creation");
		open_pop = false;
	}

	if (ImGui::BeginPopup("node_creation"))
	{
		//Try to set this in a pop up, spoiler works but not easy to close combo.
		static const char* node_types[] = { "No type selected","InputKeyboard", "MouseMotion", "TranslateGameObject","NodeRotateGameObject", "NodeInputMouse","InstatiateGameObject" };
		static int current_type = 0;

		if (ImGui::Combo("Select type of new node: ", &current_type, node_types, ((int)(sizeof(node_types) / sizeof(*node_types)))))
		{
			CreateNodeByType((NodeSubType)current_type);
		}
		ImGui::EndPopup();
	}
		
	

	ImGui::PopStyleVar();


	//-------------------------------------------------------------------------------------------------------

	//Scroll Canvas------------------------------------------------------------------------------------------
	if (ImGui::IsWindowHovered() && !ImGui::IsAnyItemActive() && ImGui::IsMouseDragging(2, 0.0f))
		scrolling = scrolling - ImGui::GetIO().MouseDelta;
	//-------------------------------------------------------------------------------------------------------

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();
	//----------------------------------------------------------------------------------------------------------

	ImGui::End();

	if (need_save)
	{
		SaveGraph();
		need_save = false;
	}

}

Node* NodeGraph::CreateNodeByType(NodeSubType current_type)
{
	Node* node = nullptr;

	if (current_type != 0)
	{
		switch (current_type)
		{
		case 1:
			node = (Node*)new NodeInputKeyboard(nodes.size());
			break;
		case 2:
			node = (Node*)new NodeMouseMotion(nodes.size());
			break;
		case 3:
			node = (Node*)new NodeTranslateGameObject(nodes.size());
			break;
		case 4:
			node = (Node*)new NodeRotateGameObject(nodes.size());
			break;
		case 5:
			node = (Node*)new NodeInputMouse(nodes.size());
			break;
		case 6:
			node = (Node*)new NodeInstatiateGameObject(nodes.size());
			break;
		}

		if (node != nullptr)
			nodes.push_back(node);
	}


	return node;
}

void NodeGraph::SetBackgroundNodeType(Node* node, ImDrawList* draw_list, ImVec2 node_rect_min, ImVec2 node_rect_max)
{
	int flags = 0;
	flags |= ImDrawCornerFlags_TopLeft;
	flags |= ImDrawCornerFlags_TopRight;


	switch (node->type)
	{
	case NodeType::EventType:

		draw_list->AddRectFilled(node_rect_min, ImVec2(node_rect_max.x, node_rect_min.y + 20), IM_COL32(255, 0, 0, 60), 4.0f, flags);
		draw_list->AddText(ImVec2((node_rect_min.x + (node_rect_max.x - node_rect_min.x) / 4), node_rect_min.y + 5), IM_COL32(255, 255, 255, 255), "Event: ");

		break;
	case NodeType::FunctionType:

		draw_list->AddRectFilled(node_rect_min, ImVec2(node_rect_max.x, node_rect_min.y + 20), IM_COL32(0, 255, 0, 60), 4.0f, flags);
		draw_list->AddText(ImVec2((node_rect_min.x + (node_rect_max.x - node_rect_min.x) / 4), node_rect_min.y + 5), IM_COL32(255, 255, 255, 255), "Function: ");

		break;
	case NodeType::DefaultType:
		draw_list->AddRectFilled(node_rect_min, ImVec2(node_rect_max.x, node_rect_min.y + 20), IM_COL32(0, 0, 255, 60), 4.0f, flags);
		draw_list->AddText(ImVec2(( node_rect_min.x + (node_rect_max.x-node_rect_min.x)/4) , node_rect_min.y + 5), IM_COL32(255, 255, 255, 255), "Default: ");

		break;
	}
}
void NodeGraph::DeleteNode(Node& node)
{
	std::vector<Node*>::iterator it = nodes.begin() + node.id;

	uint id_erased = node.id;

	for (int i = 0; i < links.size(); i++)
	{
		if (links[i].input_index == id_erased)
		{
			std::vector<NodeGraphicalLink>::iterator link_erase = links.begin() + i;
			links.erase(link_erase);
			links.shrink_to_fit();

			if (links.size() > 0)
				i = 0;
			else
				break;
		}

		if (links[i].output_index == id_erased)
		{
			std::vector<NodeGraphicalLink>::iterator link_erase = links.begin() + i;
			links.erase(link_erase);
			links.shrink_to_fit();
			i = 0;
		}


	}

	nodes.erase(it);
	nodes.shrink_to_fit();


	for (int i = 0; i < nodes.size(); ++i)
	{
		nodes[i]->id = i;
		
		for (int j = 0; j < nodes[i]->outputs_vec.size(); j++)
		{
			if (nodes[i]->outputs_vec[j] == &node)
			{
				it = nodes[i]->outputs_vec.begin() + j;
				nodes[i]->outputs_vec.erase(it);
				nodes[i]->outputs_vec.shrink_to_fit();

			}
		}

		for (int j = 0; j < nodes[i]->inputs_vec.size(); j++)
		{
			if (nodes[i]->inputs_vec[j] == &node)
			{
				it = nodes[i]->inputs_vec.begin() + j;
				nodes[i]->inputs_vec.erase(it);
				nodes[i]->inputs_vec.shrink_to_fit();
			}
		}

	}

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------------------

//Node----------------------------------------------------------------------------------------------------------------------------------------------------------------
void Node::SetNodeReferencesInJSON(JSON_Object* obj) {

	json_object_set_number(obj, "id", id);
	
}

void Node::DrawNode()
{

}

bool Node::Update()
{
	return returned_result;
}

void Node::SaveNodeInformation(JSON_Object* obj)
{
	json_object_set_number(obj,"Type",type);
	json_object_set_number(obj, "Subtype", subtype);

	json_object_set_number(obj, "Px", position.x);
	json_object_set_number(obj, "Py", position.y);

	JSON_Value* arr_inputs = json_value_init_array();
	JSON_Array* inputs_array = json_value_get_array(arr_inputs);

	for (uint i = 0; i < inputs_vec.size(); ++i)
	{
		json_array_append_number(inputs_array, inputs_vec[i]->id);
	}

	json_object_set_value(obj, "Inputs", arr_inputs);

	JSON_Value* arr_outputs = json_value_init_array();
	JSON_Array* outputs_array = json_value_get_array(arr_outputs);

	for (uint i = 0; i < outputs_vec.size(); ++i)
	{
		json_array_append_number(outputs_array, outputs_vec[i]->id);
	}

	json_object_set_value(obj, "Outputs", arr_outputs);


}

void Node::GetNodeReferencesInJSON(JSON_Object* obj)
{

}

//-----------------------------------------------------------------------------------------------------------------------------------------