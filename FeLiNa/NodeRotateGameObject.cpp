#include "NodeRotateGameObject.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ComponentTransform.h"

NodeRotateGameObject::NodeRotateGameObject(int id) : Node(id, "Game object to translate:", { 100,100 }, 1, 1, NodeType::FunctionType)
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

			if (x_motion > 0)
			{
				rot += { 1, 0, 0 };
			}
			else
			{
				rot += { -1, 0, 0 };
			}

			if (y_motion > 0)
			{
				rot += { 0, 0, 1 };
			}
			else
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