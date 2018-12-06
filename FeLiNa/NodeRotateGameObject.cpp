#include "NodeRotateGameObject.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ComponentTransform.h"

NodeRotateGameObject::NodeRotateGameObject(int id) : Node(id, "Game object to rotate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{
	subtype = NodeSubType::RotateGO;
}


bool NodeRotateGameObject::Update()
{
	returned_result = false;

	if (go != nullptr)
	{
		if (go->transform != nullptr)
		{
			returned_result = true;


			go->transform->SumRotation(axis);

		}
	}


	return returned_result;
}



void NodeRotateGameObject::DrawNode()
{
	if (go == nullptr)
		ImGui::Text("Not valid game object");
	else
		ImGui::Text(go->GetName());

	ImGui::InputFloat3("Axis to rotate", &axis[0], 2);

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