#include "NodeInstatiateGameObject.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ComponentSpeed.h"
#include "Application.h"
#include "SceneSerialization.h"
#include "ModuleScene.h"
#include "mmgr/mmgr.h"
NodeInstatiateGameObject::NodeInstatiateGameObject(int id) : Node(id, "Game object to instantiate:", { 100,100 }, 1, 1, NodeType::FunctionType)
{
	subtype = NodeSubType::InstatiateGO;
}

NodeInstatiateGameObject::~NodeInstatiateGameObject()
{

}

bool NodeInstatiateGameObject::Update(float dt)
{
	returned_result = false;

	if (instance_fbx_path.size() != 0)
	{
		returned_result = true;
		std::string path = instance_fbx_path;
		GameObject* instance = App->serialization_scene->LoadGOFromJson((char*)path.c_str());


		if (get_position_from_GO) {
			instance->transform->SetPosition(GO_position->transform->GetGlobalPosition());
		}
		else {
			instance->transform->SetPosition(new_pos);
		}


		instance->SetParent(App->scene->root_object);


		if (instance->transform == nullptr) {
			instance->AddComponent(Component_Transform);
		}

		if (instance->speed == nullptr) {
			instance->AddComponent(Component_Speed);
		}

		if (get_speed_dir_from_GO) {

			math::float3 rotated_speed = GO_position->transform->GetTransformMatrix().WorldZ();

			if (rectificate_x)
				rotated_speed.x = 0;
			if (rectificate_y)
				rotated_speed.y = 0;
			if (rectificate_z)
				rotated_speed.z = 0;

			instance->speed->SetDirection(rotated_speed.Normalized());
			instance->speed->SetVelocity(velocity);
		}
		else {

			instance->speed->SetDirection(direction);
			instance->speed->SetVelocity(velocity);
		}
	}

	return returned_result;
}


void NodeInstatiateGameObject::DrawNode()
{
	if (instance_fbx_path.size() == 0)
		ImGui::Text("Not valid game object");
	else
		ImGui::Text(instance_fbx_path.c_str());
	ImGui::SameLine();
	ImGui::Button("Drag FBX Here");

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Assets_Nodes"))
		{
			instance_fbx_path = (char*)payload->Data;
			instance_fbx_path.erase(instance_fbx_path.find_last_of("."), instance_fbx_path.size());
		}
		ImGui::EndDragDropTarget();
	}


	ImGui::Checkbox("Get position from Game Object", &get_position_from_GO);

	if(!get_position_from_GO)
	ImGui::InputFloat3("Instance pos", &new_pos[0], 2);
	else {
		ImGui::Text(position_GO_name.c_str());
		ImGui::SameLine();
		ImGui::Button("Drag Game Object Here");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_nodes"))
			{
				GO_position = *(GameObject**)payload->Data;
				if (GO_position->transform != nullptr) { 
					position_GO_name.clear();
					position_GO_name = GO_position->GetName();
				}
			}
			ImGui::EndDragDropTarget();
		}
	}
	
	

	ImGui::Checkbox("Get speed axis rotation from Game Object ", &get_speed_dir_from_GO);
	


	if (get_speed_dir_from_GO) {

		ImGui::Text(speed_dir_GO_name.c_str());
		ImGui::SameLine();
		ImGui::Button("Drag Game Object Here");

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Hierarchy_nodes"))
			{
				GO_speed_dir = *(GameObject**)payload->Data;
				if (GO_speed_dir->transform != nullptr) {
				
					speed_dir_GO_name = GO_speed_dir->GetName();

				}
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Text("Set any axis to 0?");
		ImGui::SameLine();
		ImGui::Checkbox("X", &rectificate_x);
		ImGui::SameLine();
		ImGui::Checkbox("Y", &rectificate_y);
		ImGui::SameLine();
		ImGui::Checkbox("Z", &rectificate_z);

		ImGui::InputFloat("Velocity", &velocity);
	}
	else
	{
		ImGui::DragFloat3("Direction", &direction[0], 0.01f, 0.0F, 1.0F);
		ImGui::InputFloat("Velocity", &velocity);
	}

	


}

void NodeInstatiateGameObject::SetNodeReferencesInJSON(JSON_Object* obj)
{
	json_object_set_number(obj, "id", id);
	json_object_set_number(obj, "posx", new_pos.x);
	json_object_set_number(obj, "posy", new_pos.y);
	json_object_set_number(obj, "posz", new_pos.z);

	json_object_set_boolean(obj, "rectificate x", rectificate_x);
	json_object_set_boolean(obj, "rectificate y", rectificate_y);
	json_object_set_boolean(obj, "rectificate z", rectificate_z);

	json_object_set_number(obj, "velocity", velocity);

	if (GO_position != nullptr)
		json_object_set_number(obj, "position uid", GO_position->uid);
	else
		json_object_set_number(obj, "position uid", 0);

	json_object_set_number(obj, "dirx", direction.x);
	json_object_set_number(obj, "diry", direction.y);
	json_object_set_number(obj, "dirz", direction.z);

	if(GO_speed_dir != nullptr)
		json_object_set_number(obj, "speed dir uid", GO_speed_dir->uid);
	else
		json_object_set_number(obj, "speed dir uid", 0);

	
	json_object_set_string(obj, "FBX path", instance_fbx_path.c_str());
	

}

void NodeInstatiateGameObject::GetNodeReferencesInJSON(JSON_Object* obj)
{
	new_pos.x = json_object_get_number(obj, "posx");
	new_pos.y = json_object_get_number(obj, "posy");
	new_pos.z = json_object_get_number(obj, "posz");

	direction.x = json_object_get_number(obj, "dirx");
	direction.y = json_object_get_number(obj, "diry");
	direction.z = json_object_get_number(obj, "dirz");

	rectificate_x = json_object_get_boolean(obj, "rectificate x");
	rectificate_y = json_object_get_boolean(obj, "rectificate y");
	rectificate_z = json_object_get_boolean(obj, "rectificate z");

	velocity = json_object_get_number(obj, "velocity");

	instance_fbx_path = json_object_get_string(obj, "FBX path");

    int GO_speed_dir_uid = json_object_get_number(obj, "speed dir uid");
	if (GO_speed_dir_uid != 0) {
		GO_speed_dir = App->scene->root_object->SearchGOForUID(GO_speed_dir_uid);
		get_speed_dir_from_GO = true;
		speed_dir_GO_name = GO_speed_dir->GetName();
	}

	else {
		get_speed_dir_from_GO = false;
	}

	int GO_position_uid = json_object_get_number(obj, "position uid");
	if (GO_position_uid != 0) {
		GO_position = App->scene->root_object->SearchGOForUID(GO_position_uid);
		get_position_from_GO = true;
		position_GO_name = GO_position->GetName();
	}

	else {
		get_position_from_GO = false;
	}

}

void NodeInstatiateGameObject::SetReferencesNodeDuplicated(Node& node)
{
	new_pos = ((NodeInstatiateGameObject*)&node)->new_pos;
	direction = ((NodeInstatiateGameObject*)&node)->direction;
	instance_fbx_path = ((NodeInstatiateGameObject*)&node)->instance_fbx_path;
}