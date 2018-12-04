#include "NodeRotateGameObject.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ComponentTransform.h"

NodeRotateGameObject::NodeRotateGameObject(int id) : Node(id, "Game object to rotate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{

}


bool NodeRotateGameObject::Update()
{
	bool ret = false;

	if (go != nullptr)
	{
		if (go->transform != nullptr)
		{
			ret = true;

			int x_motion = App->input->GetMouseXMotion();
			int y_motion = App->input->GetMouseYMotion();

			math::float3 rot = math::float3::zero;

			if (x_motion > min_mouse_x)
			{
				rot += { 0.1, 0, 0 };
			}
			else if(x_motion < max_mouse_x)
			{
				rot += { -1, 0, 0 };
			}

			if (y_motion > min_mouse_y)
			{
				rot += { 0, 0, 1 };
			}
			else if(y_motion < max_mouse_y)
			{
				rot += { 0, 0, -1 };
			}

			go->transform->SumRotation(rot);

		}
	}


	return ret;
}



void NodeRotateGameObject::DrawNode()
{
	if (go == nullptr)
		ImGui::Text("Not valid game object");
	else
		ImGui::Text(go->GetName());

	ImGui::InputInt("Min angle x", &min_mouse_x);
	ImGui::InputInt("Max angle x", &max_mouse_x);

	ImGui::InputInt("Min angle y", &min_mouse_y);
	ImGui::InputInt("Max angle y", &max_mouse_y);

	ImGui::Text("GameObject:");
	ImGui::Button("Drag Game Object Here");

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_nodes"))
		{
			go = *(GameObject**)payload->Data;
		}
		ImGui::EndDragDropTarget();
	}


}