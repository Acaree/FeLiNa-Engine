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

void ComponentTransform::DrawInspector()
{

	if (ImGui::CollapsingHeader("Transform")) {

		if (ImGui::CollapsingHeader("Position:")) {

			char Pos_x_c[16] = {};

			sprintf_s(Pos_x_c, "%.2f", position.x);

			if (ImGui::InputText("x", Pos_x_c, sizeof(Pos_x_c))) {

				float temp = atof(Pos_x_c);

				position.x = temp;

			}



			char Pos_y_c[16] = {};

			sprintf_s(Pos_y_c, "%.2f", position.y);

			if (ImGui::InputText("y", Pos_y_c, sizeof(Pos_y_c))) {

				float temp = atof(Pos_y_c);

				position.y = temp;

			}


			char Pos_z_c[16] = {};

			sprintf_s(Pos_z_c, "%.2f", position.z);

			if (ImGui::InputText("z", Pos_z_c, sizeof(Pos_z_c))) {

				float temp = atof(Pos_z_c);

				position.z = temp;
			}
		}

		if (ImGui::CollapsingHeader("Rotation:")) {


			char Rot_x_c[16] = {};

			sprintf_s(Rot_x_c, "%.2f", euler_angles.x);

			if (ImGui::InputText("x", Rot_x_c, sizeof(Rot_x_c))) {

				float temp = atof(Rot_x_c);

				euler_angles.x = temp;

			}



			char Rot_y_c[16] = {};

			sprintf_s(Rot_y_c, "%.2f", euler_angles.y);

			if (ImGui::InputText("y", Rot_y_c, sizeof(Rot_y_c))) {

				float temp = atof(Rot_y_c);

				euler_angles.y = temp;

			}


			char Rot_z_c[16] = {};

			sprintf_s(Rot_z_c, "%.2f", euler_angles.z);

			if (ImGui::InputText("z", Rot_z_c, sizeof(Rot_z_c))) {

				float temp = atof(Rot_z_c);

				euler_angles.z = temp;

			}

		}

		if (ImGui::CollapsingHeader("Scale:")) {


			char Sc_x_c[16] = {};

			sprintf_s(Sc_x_c, "%.2f", scale.x);

			if (ImGui::InputText("x", Sc_x_c, sizeof(Sc_x_c))) {

				float temp = atof(Sc_x_c);
				scale.x = temp;

			}



			char Sc_y_c[16] = {};

			sprintf_s(Sc_y_c, "%.2f", scale.y);

			if (ImGui::InputText("y", Sc_y_c, sizeof(Sc_y_c))) {

				float temp = atof(Sc_y_c);

				scale.y = temp;

			}


			char Sc_z_c[16] = {};

			sprintf_s(Sc_z_c, "%.2f", scale.z);

			if (ImGui::InputText("z", Sc_z_c, sizeof(Sc_z_c))) {

				float temp = atof(Sc_z_c);

				scale.z = temp;

			}
		}

	}
		
	

}
