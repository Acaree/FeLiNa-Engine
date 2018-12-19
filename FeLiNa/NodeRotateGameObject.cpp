#include "NodeRotateGameObject.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ComponentTransform.h"
#include "ModuleScene.h"

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

void NodeRotateGameObject::SetNodeReferencesInJSON(JSON_Object* obj) {

	json_object_set_number(obj, "id", id);
	json_object_set_number(obj, "rotx", axis.x);
	json_object_set_number(obj, "roty", axis.y);
	json_object_set_number(obj, "rotz", axis.z);
	if (go != nullptr)
		json_object_set_number(obj, "GO uid", go->uid);
	else
		json_object_set_number(obj, "GO uid", 0);

}

void NodeRotateGameObject::GetNodeReferencesInJSON(JSON_Object* obj)
{
	axis.x = json_object_get_number(obj, "rotx");
	axis.y = json_object_get_number(obj, "roty");
	axis.z = json_object_get_number(obj, "rotz");

	int go_uid = json_object_get_number(obj, "GO uid");

	if (go_uid != 0)
		go = App->scene->root_object->SearchGOForUID(go_uid);
}

void NodeRotateGameObject::SetReferencesNodeDuplicated(Node& node)
{
	axis = ((NodeRotateGameObject*)&node)->axis;
	go = ((NodeRotateGameObject*)&node)->go;
}