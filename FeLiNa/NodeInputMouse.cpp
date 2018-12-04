#include "NodeInputMouse.h"
#include "Application.h"
#include "ModuleInput.h"


NodeInputMouse::NodeInputMouse(int id) : Node(id, "Mouse click button:", { 100,100 }, 1, 1, NodeType::EventType)
{

}

bool NodeInputMouse::Update()
{
	bool ret = false;

	if (key_code > 0 && key_code <= 3)
	{
		if (App->input->GetMouseButton(key_code) == KEY_DOWN)
			ret = true;
	}

	return ret;
}

void NodeInputMouse::DrawNode()
{
	ImGui::Text("1: Left 2: Middle 3: Right ");

	ImGui::InputInt("Mouse Button:", &key_code, 1);

	
}