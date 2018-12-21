#ifndef _NODE_GRAPH_
#define _NODE_GRAPH_

#include "Application.h"
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


enum NodeType
{
	EventType = 0,
	FunctionType,
	DefaultType
};

enum NodeSubType
{
	InputKeyboard = 1,
	MouseMotion,
	TranslateGO,
	RotateGO,
	InputMouse,
	InstatiateGO,
	DefaultSubType
};



class Node
{
public:

	char name[DEFAULT_BUF_SIZE];
	int id = -1;

	int input_counts = 0;
	int output_counts = 0;
	NodeType type = NodeType::DefaultType;
	NodeSubType subtype = NodeSubType::DefaultSubType;
	ImVec2 position, size;

	//Not work for now----------------------
	std::vector<Node*> inputs_vec;
	std::vector<Node*> outputs_vec;
	//--------------------------------------

	Node(int Id, char* Name, ImVec2 Position, int Inputs_counts, int Output_counts, NodeType Type = NodeType::DefaultType) { id = Id; strcpy(name, Name); position = Position; input_counts = Inputs_counts; output_counts = Output_counts; type = Type; };
	ImVec2 GetInputSlotPos(int slot_no) const { return ImVec2(position.x, position.y + size.y * ((float)slot_no + 1) / ((float)input_counts + 1)); }
	ImVec2 GetOutputSlotPos(int slot_no) const { return ImVec2(position.x + size.x, position.y + size.y * ((float)slot_no + 1) / ((float)output_counts + 1)); }

	bool returned_result = false;
	bool active = true;
	

	virtual bool Update(float dt);
	virtual void DrawNode();

	void SaveNodeInformation(JSON_Object* obj);
	virtual void SetReferencesNodeDuplicated(Node& node);
	virtual void SetNodeReferencesInJSON(JSON_Object* obj);
	virtual void GetNodeReferencesInJSON(JSON_Object* obj);

};

struct NodeGraphicalLink
{
	int input_index = 0, input_slots = 0, output_index = 0, output_slots = 0;

	NodeGraphicalLink(int Input_index, int Input_slots, int Output_index, int Output_slots) { input_index = Input_index; input_slots = Input_slots; output_index = Output_index; output_slots = Output_slots; };

};


class NodeGraph
{
public:

	NodeGraph(uint uid = 0, const char* name = "No Name");

	bool Update(float dt);
	void DrawNodeGraph();
	void SetBackgroundNodeType(Node& node, ImDrawList& draw_list, ImVec2& node_rect_min, ImVec2& node_rect_max);
	void DeleteNode(Node& node);
	Node* CreateNodeByType(NodeSubType type);
	void CreateNewLink(int& input_node_pos, int& output_node_pos);
	void DeleteLink(int& node_ids, int& slot_idx);
	void DuplicateNode(Node& node);
	void SaveGraph();
	void LoadGraph(const char* path);

	void SaveReferences(JSON_Object* obj);
	void LoadReferences(JSON_Object* obj);
public:

	uint uid = 0;

	std::vector<Node*> nodes;
	std::vector<NodeGraphicalLink> links;
	//Node* last_input_node_clicked = nullptr;
	int input_clicked = -1;
	int input_node_pos = -1;
	//Node* last_output_node_clicked = nullptr;
	int output_clicked = -1;
	int output_node_pos = -1;
	
	//TO Revision
	char name[DEFAULT_BUF_SIZE] = "No Name";
	bool need_save = false;
};




#endif
