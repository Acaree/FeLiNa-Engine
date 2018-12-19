#include "NodeInputMouse.h"
#include "Application.h"
#include "ModuleInput.h"


NodeInputMouse::NodeInputMouse(int id) : Node(id, "Mouse click button:", { 100,100 }, 0, 1, NodeType::EventType)
{
	subtype = NodeSubType::InputMouse;
}

bool NodeInputMouse::Update()
{
	returned_result = false;

	if (key_code > 0 && key_code <= 3)
	{
		if (App->input->GetMouseButton(key_code) == KEY_DOWN)
			returned_result = true;
	}

	return returned_result;
}

void NodeInputMouse::DrawNode()
{
	ImGui::Text("1: Left 2: Middle 3: Right ");

	ImGui::InputInt("Mouse Button:", &key_code, 1);

	
}

void NodeInputMouse::SetNodeReferencesInJSON(JSON_Object* obj)
{
	json_object_set_number(obj, "id", id);
	json_object_set_number(obj, "key", key_code);
}

void NodeInputMouse::GetNodeReferencesInJSON(JSON_Object* obj)
{
	key_code = json_object_get_number(obj, "key");
}

void NodeInputMouse::SetReferencesNodeDuplicated(Node& node)
{
	key_code = ((NodeInputMouse*)&node)->key_code;
}