#include "ComponentCamera.h"
#include "Component.h"
#include "GameObject.h"

ComponentCamera::ComponentCamera(GameObject* go) : Component(go)
{
	type = Component_Camera;

	frustum.type = PerspectiveFrustum;
	frustum.pos = float3::zero;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;

	frustum.nearPlaneDistance = 0.1F;
	frustum.farPlaneDistance = 1000.0F;
	frustum.verticalFov = math::DegToRad(60.0F);
	frustum.horizontalFov = 2 * atanf(1.3 * tanf(frustum.verticalFov * 0.5));
}

ComponentCamera::~ComponentCamera()
{
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
	return frustum.AspectRatio;
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