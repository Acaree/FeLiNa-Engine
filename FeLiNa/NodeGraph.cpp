#include "NodeGraph.h"


void NodeGraph::AddTestNodes()
{

	nodes.push_back(Node(0, "The teacher says: ", ImVec2(40, 50), 1, 1, NodeType::EventType));
	nodes.push_back(Node(1, "You are going to create a visual programming system", ImVec2(40, 150), 1, 1, NodeType::FunctionType));
	nodes.push_back(Node(2, "you only have 2 options:", ImVec2(270, 80), 2, 2));
	nodes.push_back(Node(3, "1.Cry so hard", ImVec2(290, 80), 1, 1));
	nodes.push_back(Node(4, "2.Cry so hard ", ImVec2(330, 80), 1, 1));

	links.push_back(NodeLink(0, 0, 1, 0));
	links.push_back(NodeLink(1, 0, 2, 1));
	links.push_back(NodeLink(2, 0, 3, 0));
	links.push_back(NodeLink(2, 1, 4, 0));
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
		Node* node = &nodes[node_ids];
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

	ImGui::Text("Canvas");
	ImGui::SameLine(ImGui::GetWindowWidth() - 220);
	ImGui::Checkbox("Show grid", &show_grid);
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
		NodeLink* link = &links[link_idex];
		Node* node_inp = &nodes[link->input_index];
		Node* node_out = &nodes[link->output_index];
		ImVec2 p1 = offset + node_inp->GetOutputSlotPos(link->input_slots);
		ImVec2 p2 = offset + node_out->GetInputSlotPos(link->output_slots);
		draw_list->AddBezierCurve(p1, p1 + ImVec2(+50, 0), p2 + ImVec2(-50, 0), p2, IM_COL32(200, 200, 100, 255), 3.0f);
	}
	//----------------------------------------------------------------------------------------------------


	//Print Nodes--------------------------------------------------------------------------------------------
	for (int node_ids = 0; node_ids < nodes.size(); node_ids++)
	{
		Node* node = &nodes[node_ids];
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
		
		draw_list->AddRectFilled(node_rect_min, node_rect_max, node_bg_color, 4.0f);
		
		SetBackgroundNodeType(node, draw_list, node_rect_min, node_rect_max);

		draw_list->AddRect(node_rect_min, node_rect_max, IM_COL32(100, 100, 100, 255), 4.0f);
	

		for (int slot_idx = 0; slot_idx < node->input_counts; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetInputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));
		for (int slot_idx = 0; slot_idx < node->output_counts; slot_idx++)
			draw_list->AddCircleFilled(offset + node->GetOutputSlotPos(slot_idx), NODE_SLOT_RADIUS, IM_COL32(150, 150, 150, 150));


		ImGui::PopID();
	}
	
	draw_list->ChannelsMerge();
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

