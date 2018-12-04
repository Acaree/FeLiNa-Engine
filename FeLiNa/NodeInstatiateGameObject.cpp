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
	bool ret = false;

	if (go != nullptr)
	{
		std::string path = "Assets/";
		path += go->GetName();
		App->serialization_scene->LoadScene((char*)path.c_str());
	}

	if (ret)
	{
		for (uint i = 0; i < outputs_vec.size(); ++i)
		{
			ret = outputs_vec[i]->Update();

			if (!ret)
				break;
		}
	}

	return ret;
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