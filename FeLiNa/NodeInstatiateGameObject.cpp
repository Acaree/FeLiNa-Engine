#include "NodeInstatiateGameObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "SceneSerialization.h"
#include "ModuleScene.h"
NodeInstatiateGameObject::NodeInstatiateGameObject(int id) : Node(id, "Game object to instantiate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{
	subtype = NodeSubType::InstatiateGO;
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
	ImGui::InputFloat("###positioninstantiate", &new_pos[0]);
	

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

void NodeInstatiateGameObject::SetNodeReferencesInJSON(JSON_Object* obj)
{
	json_object_set_number(obj, "id", id);
	json_object_set_number(obj, "posx", new_pos.x);
	json_object_set_number(obj, "posy", new_pos.y);
	json_object_set_number(obj, "posz", new_pos.z);
	json_object_set_number(obj, "GO uid", go->uid);

}

void NodeInstatiateGameObject::GetNodeReferencesInJSON(JSON_Object* obj)
{
	new_pos.x = json_object_get_number(obj, "posx");
	new_pos.y = json_object_get_number(obj, "posy");
	new_pos.z = json_object_get_number(obj, "posz");

	int go_uid = json_object_get_number(obj, "GO uid");

	if(go_uid != 0)
		go = App->scene->root_object->SearchGOForUID(go_uid);

}