#include "ComponentTransform.h"
#include "Globals.h"
#include "ImGui/imgui.h"
#include "GameObject.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "GameObject.h"
#include "ComponentCamera.h"
#include "mmgr/mmgr.h"
ComponentTransform::ComponentTransform(GameObject* parent, math::float3 position, math::float3 rotation, math::float3 scale): Component(parent)
{
	type = Component_Transform;

	this->position = position;

	euler_angles = rotation;

	quat_rotation = math::Quat::FromEulerXYZ(euler_angles.x * DEGTORAD, euler_angles.y * DEGTORAD, euler_angles.z * DEGTORAD);

	this->scale = scale;

	UpdateMatrix();


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

		if (parent_trans != nullptr)
		{
			math::float4x4 tempmatrix = parent_trans->GetTransformMatrix();

			global_matrix = tempmatrix * local_matrix;
		}
		else
		{
			global_matrix = local_matrix;
		}

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
	quat_rotation = quat_rotation * math::Quat({ rot.x,rot.y,rot.z,1 });
}

void ComponentTransform::SumScale(math::float3 scale)
{
	this->scale.x *= scale.x;
	this->scale.y *= scale.y;
	this->scale.z *= scale.z;
}


void ComponentTransform::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);

	json_object_set_number(obj, "px", position.x);
	json_object_set_number(obj, "py", position.y);
	json_object_set_number(obj, "pz", position.z);

	json_object_set_number(obj, "qx", quat_rotation.x);
	json_object_set_number(obj, "qy", quat_rotation.y);
	json_object_set_number(obj, "qz", quat_rotation.z);
	json_object_set_number(obj, "qw", quat_rotation.w);

	json_object_set_number(obj, "sx", scale.x);
	json_object_set_number(obj, "sy", scale.y);
	json_object_set_number(obj, "sz", scale.z);
}

void ComponentTransform::OnLoad(JSON_Object* obj)
{
	position.x = json_object_get_number(obj, "px");
	position.y = json_object_get_number(obj, "py");
	position.z = json_object_get_number(obj, "pz");

	quat_rotation.x = json_object_get_number(obj, "rx");
	quat_rotation.y = json_object_get_number(obj, "ry");
	quat_rotation.z = json_object_get_number(obj, "rz");
	quat_rotation.w = json_object_get_number(obj, "rw");

	scale.x = json_object_get_number(obj, "sx");
	scale.y = json_object_get_number(obj, "sy");
	scale.z = json_object_get_number(obj, "sz");

	
}


void ComponentTransform::DrawInspector()
{

	ShowGuizmos();

	if (ImGui::TreeNodeEx("Transform"))
	{
		ImGui::Text("Position:");
		ImGui::SameLine();

		math::float3 last_position = position;
		math::float3 last_rotation = euler_angles;
		math::float3 last_scale = scale;

		if (ImGui::DragFloat3("##pos", &last_position[0], 1.0F))
		{
			if(!parent->static_object)
				SetPosition(last_position);
		}

		ImGui::Text("Rotation:");
		ImGui::SameLine();

		if (ImGui::DragFloat3("##rot", &last_rotation[0], 1.0F))
		{
			if(!parent->static_object)
				SetRotation(last_rotation);
		}

		ImGui::Text("Scale:");
		ImGui::SameLine();

		if (ImGui::DragFloat3("##scale", &last_scale[0], 1.0F))
		{
			if(!parent->static_object)
				SetScale(last_scale);
		}

		ImGui::TreePop();
	}
	
	
}

math::float4x4 ComponentTransform::GetGlobalMatrix() {

	return global_matrix;
}


void ComponentTransform::ShowGuizmos()
{
	ImGuizmo::Enable(true);

	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

	static ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {

		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	}

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN) {

		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	}

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN) {

		mCurrentGizmoOperation = ImGuizmo::SCALE;
	}
	math::float4x4 matrix = global_matrix.Transposed();

	ImGuizmo::Manipulate(App->camera->current_camera->GetViewMatrix(), App->camera->current_camera->GetProjectionMatrix(), mCurrentGizmoOperation, ImGuizmo::WORLD, matrix.ptr());

	matrix.Transpose();

	if (ImGuizmo::IsUsing() && parent->static_object == false)
	{
		if (mCurrentGizmoOperation == ImGuizmo::SCALE) {

			math::Quat despreciable_rot;
			matrix.Decompose(position, despreciable_rot, scale);
			UpdateMatrix();

		}

		else {
			matrix.Decompose(position, quat_rotation, scale);
			euler_angles = quat_rotation.ToEulerXYZ() * RADTODEG;
			UpdateMatrix();
		}
	}

}

