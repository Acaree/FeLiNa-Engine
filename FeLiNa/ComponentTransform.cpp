#include "ComponentTransform.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(GameObject* parent, math::float3 position, math::float3 rotation, math::float3 scale): Component(parent)
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

math::float4x4 ComponentTransform::GetTransformMatrix() const {

	return global_matrix;
}

void ComponentTransform::SetPosition(math::float3 new_pos) {

	position = new_pos;
	
}
void ComponentTransform::SetRotation(math::float3 new_rotation) {
	
	euler_angles = new_rotation;

	quat_rotation = math::Quat::FromEulerXYZ(euler_angles.x * DEGTORAD, euler_angles.y * DEGTORAD, euler_angles.z * DEGTORAD);
}

void ComponentTransform::SetScale(math::float3 new_scale) {

	scale = new_scale;

}

math::float3 ComponentTransform::GetPosition()const{

	return position;
}
math::float3 ComponentTransform::GetRotation()const{

	return euler_angles;
}
math::float3 ComponentTransform::GetScale()const{

	return scale;
}

void ComponentTransform :: UpdateMatrix() {

	//quat_rotation = math::Quat::FromEulerXYZ(euler_angles.x * DEGTORAD, euler_angles.y * DEGTORAD, euler_angles.z * DEGTORAD);

	local_matrix = math::float4x4::FromTRS(position,math::Quat(quat_rotation.x,quat_rotation.y,quat_rotation.z,quat_rotation.w), scale);

	if (parent->GetParent() != nullptr) {
		ComponentTransform* parent_trans = (ComponentTransform*)parent->GetParent()->GetComponent(Component_Transform);

		math::float4x4 tempmatrix = parent_trans->GetTransformMatrix();

		global_matrix = tempmatrix * local_matrix;

	}

	else {
		
		global_matrix = local_matrix;

	}
}

void ComponentTransform::SetQuaternion(math::Quat quaternion)
{
	quat_rotation = quaternion;
}

math::Quat ComponentTransform::GetQuaternion() const
{
	return quat_rotation;
}

