#include "ComponentCamera.h"
#include "Component.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Glew/include/glew.h"
#include "ImGui/imgui.h"

ComponentCamera::ComponentCamera(GameObject* go) : Component(go)
{
	type = Component_Camera;

	frustum.type = math::PerspectiveFrustum;
	frustum.pos = math::float3::zero;
	frustum.front = math::float3::unitZ;
	frustum.up = math::float3::unitY;

	frustum.nearPlaneDistance = 1.0F;
	frustum.farPlaneDistance = 100.0F;
	frustum.verticalFov = math::DegToRad(60.0F);
	frustum.horizontalFov = 2 * atanf(1.3 * tanf(frustum.verticalFov * 0.5));
}

ComponentCamera::~ComponentCamera()
{
	

}

void ComponentCamera::Update(float dt)
{
	const ComponentTransform* transform = (ComponentTransform*)parent->GetComponent(Component_Transform);

	math::float4x4 matrix = math::float4x4::identity;

	if (transform != nullptr) {
		matrix = transform->GetTransformMatrix();
	}
	frustum.pos = matrix.TranslatePart();
	frustum.front = matrix.WorldZ().Normalized();
	frustum.up = frustum.front.Cross(-frustum.WorldRight()).Normalized();

	//DebugDraw();
}

float ComponentCamera::GetNear() const
{
	return frustum.nearPlaneDistance;
}

float ComponentCamera::GetFar() const
{
	return frustum.farPlaneDistance;
}

float ComponentCamera::GetFov() const
{
	return math::RadToDeg(frustum.verticalFov);
}

float ComponentCamera::GetAspectRatio() const
{
	return frustum.AspectRatio();
}

void ComponentCamera::SetNear(float f_near)
{
	frustum.nearPlaneDistance = f_near;
}

void ComponentCamera::SetFar(float f_far)
{
	frustum.farPlaneDistance = f_far;
}

void ComponentCamera::SetFov(float f_fov)
{
	frustum.verticalFov = math::DegToRad(f_fov);
}

void ComponentCamera::SetAspectRatio(float f_ratio)
{
	frustum.horizontalFov = 2 * atanf(f_ratio * tanf(frustum.verticalFov * 0.5));
}

void ComponentCamera::DebugDraw()
{
	glBegin(GL_LINES);
	glLineWidth(50.0f);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}

bool ComponentCamera::ContainsAaBox(const math::AABB refBox) const
{
	
	bool ret = true;

	math::float3 corners[8];
	int total_in = 0;
	refBox.GetCornerPoints(corners);

	for (uint p = 0; p < 6; ++p)
	{
		int In_count = 8;
		int point_In = 1;

		for (uint i = 0; i < 8; ++i)
		{
			if (frustum.GetPlane(p).IsOnPositiveSide(corners[i]))
			{
				point_In = 0;
				In_count--;
			}
		}

		if (In_count == 0)
		{
			return false;
		}

		total_in += point_In;
	}

	return ret;
}

float* ComponentCamera::GetViewMatrix() const
{
	static math::float4x4 matrix;
	matrix = frustum.ViewMatrix();
	matrix.Transpose();

	return (float*)matrix.v;
}

float* ComponentCamera::GetProjectionMatrix() const
{
	static math::float4x4 matrix;

	matrix = frustum.ProjectionMatrix();
	matrix.Transpose();

	return (float*)matrix.v;

}


void ComponentCamera::OnSave(JSON_Object* obj)
{
	json_object_set_number(obj, "type", type);
	json_object_set_number(obj, "near", frustum.nearPlaneDistance);
	json_object_set_number(obj, "far", frustum.farPlaneDistance);
	json_object_set_number(obj, "vFov", frustum.verticalFov);
	json_object_set_number(obj, "hFov", frustum.horizontalFov);
}

void ComponentCamera::OnLoad(JSON_Object* obj)
{
	frustum.nearPlaneDistance = json_object_get_number(obj, "near");
	frustum.farPlaneDistance = json_object_get_number(obj, "far");
	frustum.verticalFov = json_object_get_number(obj, "vFov");
	frustum.horizontalFov = json_object_get_number(obj, "hFov");
}


void ComponentCamera::DrawInspector()
{
	if (ImGui::TreeNodeEx("Camera"))
	{
		ImGui::Text("Near:");
		ImGui::SameLine();

		float f_near = frustum.nearPlaneDistance;


		if (ImGui::DragFloat("##near", &f_near, 1.0F))
		{
			SetNear(f_near);
		}

		ImGui::Text("Far:");
		ImGui::SameLine();
		float f_far = frustum.farPlaneDistance;

		if (ImGui::DragFloat("##far", &f_far, 1.0F))
		{
			SetFar(f_far);
		}

		ImGui::Text("Fov:");
		ImGui::SameLine();

		float f_fov = GetFov();
		if (ImGui::DragFloat("##fov", &f_fov, 1.0F))
		{
			SetFov(f_fov);
		}

		ImGui::Checkbox("Culling", &culling);

		ImGui::TreePop();
	}
	

}