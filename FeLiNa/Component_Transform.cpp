#include "Component_transform.h"


Component_Transform::Component_Transform(GameObject* parent): Component(parent)
{
	type = ComponentTransform;
}

Component_Transform::~Component_Transform(){

}

float3x3 Component_Transform::GetTransformMatrix() const {

	return transform_matrix;
}

void Component_Transform::SetPosition(float3 new_pos) {

	position = new_pos;
	//calculate changes to rotation matrix

}
void Component_Transform::SetRotation(float3 new_rotation) {
	
	euler_angles = new_rotation;
	//calculate changes to rotation matrix

}
void Component_Transform::SetScale(float3 new_scale) {

	scale = new_scale;
}

float3 Component_Transform::GetPosition()const{

	return position;
}
float3 Component_Transform::GetRotation()const{

	return euler_angles;
}
float3 Component_Transform::GetScale()const{

	return scale;
}



