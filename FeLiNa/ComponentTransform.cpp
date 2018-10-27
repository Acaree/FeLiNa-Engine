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

	quat_rotation = math::Quat::FromEulerXYZ(euler_angles.x * DEGTORAD, euler_angles.y * DEGTORAD, euler_angles.z * DEGTORAD);

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

	if (ImGui::CollapsingHeader("Transform")) {

		ImGui::Text("Position:");

			char Pos_x_c[16] = {};

			sprintf_s(Pos_x_c, "%.2f", position.x);

			if (ImGui::InputText("x", Pos_x_c, sizeof(Pos_x_c))) {

				float temp = atof(Pos_x_c);

				position.x = temp;

				UpdateMatrix();
			}



			char Pos_y_c[16] = {};

			sprintf_s(Pos_y_c, "%.2f", position.y);

			if (ImGui::InputText("y", Pos_y_c, sizeof(Pos_y_c))) {

				float temp = atof(Pos_y_c);

				position.y = temp;

				UpdateMatrix();
			}


			char Pos_z_c[16] = {};

			sprintf_s(Pos_z_c, "%.2f", position.z);

			if (ImGui::InputText("z", Pos_z_c, sizeof(Pos_z_c))) {

				float temp = atof(Pos_z_c);

				position.z = temp;

				UpdateMatrix();
			}


			ImGui::Text("Rotation:");


			char Rot_x_c[16] = {};

			sprintf_s(Rot_x_c, "%.2f", euler_angles.x);

			if (ImGui::InputText("x", Rot_x_c, sizeof(Rot_x_c))) {

				float temp = atof(Rot_x_c);

				euler_angles.x = temp;

				UpdateMatrix();
			}



			char Rot_y_c[16] = {};

			sprintf_s(Rot_y_c, "%.2f", euler_angles.y);

			if (ImGui::InputText("y", Rot_y_c, sizeof(Rot_y_c))) {

				float temp = atof(Rot_y_c);

				euler_angles.y = temp;

				UpdateMatrix();
			}


			char Rot_z_c[16] = {};

			sprintf_s(Rot_z_c, "%.2f", euler_angles.z);

			if (ImGui::InputText("z", Rot_z_c, sizeof(Rot_z_c))) {

				float temp = atof(Rot_z_c);

				euler_angles.z = temp;

				UpdateMatrix();
			}


			ImGui::Text("Scale:");


			char Sc_x_c[16] = {};

			sprintf_s(Sc_x_c, "%.2f", scale.x);

			if (ImGui::InputText("x", Sc_x_c, sizeof(Sc_x_c))) {

				float temp = atof(Sc_x_c);
				scale.x = temp;

				UpdateMatrix();
			}



			char Sc_y_c[16] = {};

			sprintf_s(Sc_y_c, "%.2f", scale.y);

			if (ImGui::InputText("y", Sc_y_c, sizeof(Sc_y_c))) {

				float temp = atof(Sc_y_c);

				scale.y = temp;

				UpdateMatrix();
			}


			char Sc_z_c[16] = {};

			sprintf_s(Sc_z_c, "%.2f", scale.z);

			if (ImGui::InputText("z", Sc_z_c, sizeof(Sc_z_c))) {

				float temp = atof(Sc_z_c);

				scale.z = temp;

				UpdateMatrix();
			}

	}
		
	

}
