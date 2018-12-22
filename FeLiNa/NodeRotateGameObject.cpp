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

NodeRotateGameObject::~NodeRotateGameObject()
{

}

bool NodeRotateGameObject::Update(float dt)
{
	returned_result = false;

	if (go != nullptr)
	{
		if (go->transform != nullptr)
		{
			returned_result = true;

			if (rotate_with_mouse) {

				float mouse_motion = -App->input->GetMouseXMotion();

				math::float3 rot = (axis * (mouse_motion/100))*dt;
				go->transform->SumRotation(rot);
			}

			else {
				go->transform->SumRotation(axis);
			}
		}
	}

	if (returned_result) {
		for (int j = 0; j < outputs_vec.size(); j++)
		{
			if (!outputs_vec[j]->active || outputs_vec[j]->Update(dt))
				break;
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
	
	ImGui::Checkbox("Rotate with mouse motion X", &rotate_with_mouse);

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
	json_object_set_number(obj, "rot with mouse", rotate_with_mouse);
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

	rotate_with_mouse = json_object_get_number(obj, "rot with mouse");

	int go_uid = json_object_get_number(obj, "GO uid");

	if (go_uid != 0)
		go = App->scene->root_object->SearchGOForUID(go_uid);
}

void NodeRotateGameObject::SetReferencesNodeDuplicated(Node& node)
{
	axis = ((NodeRotateGameObject*)&node)->axis;
	go = ((NodeRotateGameObject*)&node)->go;
}