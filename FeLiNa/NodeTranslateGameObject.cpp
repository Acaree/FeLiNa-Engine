#include "NodeTranslateGameObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"

NodeTranslateGameObject::NodeTranslateGameObject(int id) : Node(id, "Game object to translate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{
	
}

bool NodeTranslateGameObject::Update()
{
	returned_result = false;

	if (go != nullptr)
	{
		if (go->transform != nullptr)
		{
			returned_result = true;

			go->transform->SumPosition(translation);

		}
	}

	if (returned_result)
	{
		for (uint i = 0; i < outputs_vec.size(); ++i)
		{
			returned_result = outputs_vec[i]->Update();

			if (!returned_result)
				break;
		}
	}

	return returned_result;
}


void NodeTranslateGameObject::DrawNode()
{
	if (go == nullptr)
		ImGui::Text("Not valid game object");
	else
		ImGui::Text(go->GetName());


	ImGui::Text("Speed:");
	ImGui::SameLine();

	ImGui::DragFloat3("##pos", &translation[0], 1.0F);
	
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