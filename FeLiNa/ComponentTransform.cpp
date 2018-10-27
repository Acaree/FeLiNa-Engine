#include "ComponentTransform.h"
#include "Globals.h"
#include "ImGui/imgui.h"

ComponentTransform::ComponentTransform(GameObject* parent, float3 position, float3 rotation, float3 scale): Component(parent)
{
	type = Component_Transform;

	this->position = position;

	euler_angles = rotation;

	quat_rotation = math::Quat::FromEulerXYZ(euler_angles.x * DEGTORAD, euler_angles.y * DEGTORAD, euler_angles.z * DEGTORAD);

	this->scale = scale;

	UpdateMatrix();

}

ComponentTransform::~ComponentTransform(){

}

float4x4 ComponentTransform::GetTransformMatrix() const {

	return transform_matrix;
}

void ComponentTransform::SetPosition(float3 new_pos) {

	position = new_pos;
	
	UpdateMatrix();

}
void ComponentTransform::SetRotation(float3 new_rotation) {
	
	euler_angles = new_rotation;

	quat_rotation = math::Quat::FromEulerXYZ(euler_angles.x * DEGTORAD, euler_angles.y * DEGTORAD, euler_angles.z * DEGTORAD);

	UpdateMatrix();
}

void ComponentTransform::SetScale(float3 new_scale) {

	scale = new_scale;

	UpdateMatrix();
}

float3 ComponentTransform::GetPosition()const{

	return position;
}
float3 ComponentTransform::GetRotation()const{

	return euler_angles;
}
float3 ComponentTransform::GetScale()const{

	return scale;
}

void ComponentTransform :: UpdateMatrix() {

	transform_matrix = float4x4::FromTRS(position,quat_rotation, scale);
}

void ComponentTransform::SetQuaternion(Quat quaternion)
{
	quat_rotation = quaternion;
}

Quat ComponentTransform::GetQuaternion() const
{
	return quat_rotation;
}

void ComponentTransform::DrawInspector()
{

	ImGui::Text("Position: %f %f %f",position.x,position.y,position.z);

	
	ImGui::Text("Rotation: %f %f %f", euler_angles.x, euler_angles.y, euler_angles.z);

	
	ImGui::Text("Scale: %f %f %f", scale.x, scale.y, scale.z);

}
