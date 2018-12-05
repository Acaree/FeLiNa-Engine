#include "NodeInstatiateGameObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "SceneSerialization.h"

NodeInstatiateGameObject::NodeInstatiateGameObject(int id) : Node(id, "Game object to translate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{

}

bool NodeInstatiateGameObject::Update()
{
	returned_result = false;

	if (go != nullptr)
	{
		std::string path = "Assets/";
		path += go->GetName();
		App->serialization_scene->LoadScene((char*)path.c_str());
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


void NodeInstatiateGameObject::DrawNode()
{
	if (go == nullptr)
		ImGui::Text("Not valid game object");
	else
		ImGui::Text(go->GetName());


	ImGui::Text("Instatiate pos:");
	ImGui::SameLine();

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