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

	math::float4x4 last_global = global_matrix;
	
	local_matrix = math::float4x4::FromTRS(position,quat_rotation, scale);


	if ( parent  != nullptr && parent->GetParent() != nullptr) {
		ComponentTransform* parent_trans = (ComponentTransform*)parent->GetParent()->GetComponent(Component_Transform);

		math::float4x4 tempmatrix = parent_trans->GetTransformMatrix();

		global_matrix = tempmatrix * local_matrix;

	}
	else {
		
		global_matrix = local_matrix;

	}

	
	if (!global_matrix.Equals(last_global))
		if(parent != nullptr)
			parent->RecalculateBoundingBox();

}

void ComponentTransform::SetQuaternion(math::Quat quaternion)
{
	quat_rotation = quaternion;
}

math::Quat ComponentTransform::GetQuaternion() const
{
	return quat_rotation;
}

void ComponentTransform::SumPosition(math::float3 pos)
{
	position += pos;
}

void ComponentTransform::SumRotation(math::float3 rot)
{
	euler_angles += rot;
}

void ComponentTransform::SumScale(math::float3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
}

void ComponentTransform::DrawInspector()
{
	if (ImGui::TreeNodeEx("Transform"))
	{
		ImGui::Text("Position:");
		ImGui::SameLine();

		float vel = 1.0F;

		if (parent->static_object)
			vel = 0.0F;

		if (ImGui::DragFloat3("##pos", &position[0], vel))
		{
			SetPosition(position);
		}

		ImGui::Text("Rotation:");
		ImGui::SameLine();

		if (ImGui::DragFloat3("##rot", &euler_angles[0], vel))
		{
			SetRotation(euler_angles);
		}

		ImGui::Text("Scale:");
		ImGui::SameLine();

		if (ImGui::DragFloat3("##scale", &scale[0], vel))
		{
			SetScale(scale);

		}

		ImGui::TreePop();
	}
	

}