#include "ComponentSpeed.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Application.h"
#include "mmgr/mmgr.h"


ComponentSpeed::ComponentSpeed(GameObject* parent, math::float3 speed) : Component(parent)
{
	type = Component_Speed;

	this->speed = speed;
}


void ComponentSpeed::DrawInspector()
{

	if (ImGui::TreeNodeEx("Speed"))
	{
		ImGui::Text("Speed:");
		ImGui::SameLine();


		if (ImGui::DragFloat3("##speed", &speed[0], 1.0F))
		{
			int i = 0;
		}

		ImGui::TreePop();
	}
}

void ComponentSpeed:: Update(float dt) {
	
	if (App->engine_states == ENGINE_STATE_GAME_MODE && App->game_states != GAME_STATES::ENGINE_STATE_PAUSE) {
		parent->transform->SumPosition(speed);
	}
}


void ComponentSpeed::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);

	json_object_set_number(obj, "x", speed.x);
	json_object_set_number(obj, "y", speed.y);
	json_object_set_number(obj, "z", speed.z);

}

void ComponentSpeed::OnLoad(JSON_Object* obj)
{
	speed.x = json_object_get_number(obj, "x");
	speed.y = json_object_get_number(obj, "y");
	speed.z = json_object_get_number(obj, "z");

}

void ComponentSpeed::SetSpeed(math::float3 new_speed) {

	this->speed = new_speed;

}