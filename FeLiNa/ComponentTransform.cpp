#include "ComponentTransform.h"
#include "Globals.h"


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

	transform_matrix = { 1,0,0,position.x,
						0,1,0,position.y,
						0,0,1,position.z,
						0,0,0,1 };

	
	transform_matrix = transform_matrix * quat_rotation.ToFloat4x4();

	transform_matrix = transform_matrix * float4x4{ scale.x,0,0,0,
													0,scale.y,0,0,
													0,0,scale.z,0,
													0,0,0,1 };

}



