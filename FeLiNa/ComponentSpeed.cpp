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

	this->direction = speed;
}


void ComponentSpeed::DrawInspector()
{

	if (ImGui::TreeNodeEx("Speed"))
	{
		ImGui::Text("Speed:");
		ImGui::SameLine();


		ImGui::DragFloat3("Direction", &direction[0], 0.01f,0.0F,1.0F ) ;
		ImGui::InputFloat("###velocity", &velocity);
		

		ImGui::TreePop();
	}
}

void ComponentSpeed:: Update(float dt) {
	
	if (App->engine_states == ENGINE_STATE_GAME_MODE && App->game_states != GAME_STATES::ENGINE_STATE_PAUSE) {
		;
		parent->transform->SumPosition((direction.Normalized() * (velocity*dt)));
	}
}


void ComponentSpeed::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);

	json_object_set_number(obj, "x", direction.x);
	json_object_set_number(obj, "y", direction.y);
	json_object_set_number(obj, "z", direction.z);
	json_object_set_number(obj, "vel", velocity);
}

void ComponentSpeed::OnLoad(JSON_Object* obj)
{
	direction.x = json_object_get_number(obj, "x");
	direction.y = json_object_get_number(obj, "y");
	direction.z = json_object_get_number(obj, "z");
	velocity = json_object_get_number(obj, "vel");

}

void ComponentSpeed::SetSpeed(math::float3 new_speed) {

	this->direction = new_speed;

}

void ComponentSpeed::SetVelocity(float velocity)
{
	this->velocity = velocity;
}