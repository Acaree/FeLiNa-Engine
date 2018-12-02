#ifndef _NODE_GRAPH_
#define _NODE_GRAPH_

#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include <vector>

#define IMGUI_DEFINE_MATH_OPERATORS

struct Node
{
	char name[DEFAULT_BUF_SIZE];
	int id = -1;

	int input_counts = 0;
	int output_counts = 0;

	ImVec2 position, size;

	Node(int Id, char* Name, ImVec2 Position, int Inputs_counts, int Output_counts) { id = Id; strcpy(name, Name); position = Position; input_counts = Inputs_counts; output_counts = Output_counts; };

};

struct NodeLink
{
	int input_index = 0, input_slots = 0, output_index = 0, output_slots = 0;

	NodeLink(int Input_index, int Input_slots, int Output_index, int Output_slots) { input_index = Input_index; input_slots = Input_slots; output_index = Output_index; output_slots = Output_slots; };

};


class NodeGraph
{
public:

	void AddTestNodes();

	void DrawNodeGraph();

public:
	std::vector<Node> nodes;
	std::vector<NodeLink> links;

};




#endif
