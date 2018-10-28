#include "ComponentCamera.h"
#include "Component.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "Glew/include/glew.h"

ComponentCamera::ComponentCamera(GameObject* go) : Component(go)
{
	type = Component_Camera;

	frustum.type = math::PerspectiveFrustum;
	frustum.pos = math::float3::zero;
	frustum.front = math::float3::unitZ;
	frustum.up = math::float3::unitY;

	frustum.nearPlaneDistance = 1.0F;
	frustum.farPlaneDistance = 10.0F;
	frustum.verticalFov = math::DegToRad(60.0F);
	frustum.horizontalFov = 2 * atanf(1.3 * tanf(frustum.verticalFov * 0.5));
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::Update(float dt)
{
	const ComponentTransform* transform = (ComponentTransform*)parent->GetComponent(Component_Transform);

	math::float4x4 matrix = transform->GetTransformMatrix();

	frustum.pos = matrix.TranslatePart();
	frustum.front = matrix.WorldZ().Normalized();
	frustum.up = frustum.front.Cross(-frustum.WorldRight()).Normalized();
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
	glLineWidth(10.0f);
	glColor4f(0.75f, 0.75f, 0.75f, 1.0f);

	for (uint i = 0; i < 12; i++)
	{
		glVertex3f(frustum.Edge(i).a.x, frustum.Edge(i).a.y, frustum.Edge(i).a.z);
		glVertex3f(frustum.Edge(i).b.x, frustum.Edge(i).b.y, frustum.Edge(i).b.z);
	}
	glEnd();

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

}

Culling ComponentCamera::ContainsAaBox(const math::AABB refBox) const
{
	
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
			return CULL_OUT;
		}

		total_in += point_In;
	}

	if (total_in == 6)
		return CULL_IN;
	

	return CULL_INTERSECT;
}