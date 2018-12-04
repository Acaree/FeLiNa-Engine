#include "NodeTranslateGameObject.h"
#include "GameObject.h"

NodeTranslateGameObject::NodeTranslateGameObject() : Node(3, "Game object to translate:", { 100,100 }, 0, 1, NodeType::FunctionType)
{

}

void NodeTranslateGameObject::DrawNode()
{
	if (go == nullptr)
		ImGui::Text("Not valid game object");
	else
		ImGui::Text(go->GetName());

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