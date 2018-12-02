#include "NodeGraph.h"


void NodeGraph::AddTestNodes()
{

	nodes.push_back(Node(0, "MainTex", ImVec2(40, 50), 1, 1));
	nodes.push_back(Node(1, "BumpMap", ImVec2(40, 150), 1, 1));
	nodes.push_back(Node(2, "Combine", ImVec2(270, 80), 2, 2));
	links.push_back(NodeLink(0, 0, 2, 0));
	links.push_back(NodeLink(1, 0, 2, 1));

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
	//Print Nodes--------------------------------------------------------------------------------------------
	for (int node_ids = 0; node_ids < nodes.size(); node_ids++)
	{
		Node* node = &nodes[node_ids];
		ImGui::PushID(node->id);

		node->DrawNode();

		ImGui::PopID();
	}
	
	//-------------------------------------------------------------------------------------------------------

	ImGui::PopItemWidth();
	ImGui::EndChild();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::EndGroup();
	//----------------------------------------------------------------------------------------------------------

	ImGui::End();
}

void Node::DrawNode()
{
	//This draw display the contents first.
}