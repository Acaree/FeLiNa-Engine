#include "NodeTranslateGameObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "ModuleScene.h"

NodeTranslateGameObject::NodeTranslateGameObject(int id) : Node(id, "Game object to translate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{
	subtype = NodeSubType::TranslateGO;
}

NodeTranslateGameObject::~NodeTranslateGameObject()
{

}

bool NodeTranslateGameObject::Update(float dt)
{
	returned_result = false;

	if (go != nullptr)
	{
		if (go->transform != nullptr)
		{
			returned_result = true;
			
			go->transform->SumPosition(translation*dt);

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

void NodeTranslateGameObject::SetNodeReferencesInJSON(JSON_Object* obj) {

	json_object_set_number(obj, "id", id);
	json_object_set_number(obj, "transx", translation.x);
	json_object_set_number(obj, "transy", translation.y);
	json_object_set_number(obj, "transz", translation.z);

	if (go != nullptr)
		json_object_set_number(obj, "GO uid", go->uid);
	else
		json_object_set_number(obj, "GO uid", 0);

}

void NodeTranslateGameObject::GetNodeReferencesInJSON(JSON_Object* obj)
{
	translation.x = json_object_get_number(obj, "transx");
	translation.y = json_object_get_number(obj, "transy");
	translation.z = json_object_get_number(obj, "transz");

	int go_uid = json_object_get_number(obj, "GO uid");

	if (go_uid != 0)
		go = App->scene->root_object->SearchGOForUID(go_uid);
}

void NodeTranslateGameObject::SetReferencesNodeDuplicated(Node& node)
{
	translation = ((NodeTranslateGameObject*)&node)->translation;
	go = ((NodeTranslateGameObject*)&node)->go;
}