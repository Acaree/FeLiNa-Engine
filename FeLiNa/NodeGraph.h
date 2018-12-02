#ifndef _NODE_GRAPH_
#define _NODE_GRAPH_

#include "Globals.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include <vector>
#include <math.h>

#define IMGUI_DEFINE_MATH_OPERATORS

#define GRID_COLOR IM_COL32(200, 200, 200, 40)
#define NODE_SLOT_RADIUS  4.0f
#define NODE_WINDOW_PADDING ImVec2(8.0f, 8.0f)

static inline ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x + rhs.x, lhs.y + rhs.y); }
static inline ImVec2 operator-(const ImVec2& lhs, const ImVec2& rhs) { return ImVec2(lhs.x - rhs.x, lhs.y - rhs.y); }

class Node
{
public:

	char name[DEFAULT_BUF_SIZE];
	int id = -1;

	int input_counts = 0;
	int output_counts = 0;

	ImVec2 position, size;

	Node(int Id, char* Name, ImVec2 Position, int Inputs_counts, int Output_counts) { id = Id; strcpy(name, Name); position = Position; input_counts = Inputs_counts; output_counts = Output_counts; };
	ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(position.x, position.y + size.y * ((float)slot_no + 1) / ((float)input_counts + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(position.x + size.x, position.y + size.y * ((float)slot_no + 1) / ((float)output_counts + 1)); }


	void DrawNode();

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
