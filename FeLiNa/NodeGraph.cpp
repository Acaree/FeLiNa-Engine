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

	for (int node_idx = 0; node_idx < nodes.size(); node_idx++)
	{
		Node* node = &nodes[node_idx];
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


	ImGui::End();
}