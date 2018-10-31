#ifndef _COMPONENT_CAMERA_
#define _COMPONENT_CAMERA_

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Geometry/AABB.h"

enum Culling
{
	CULL_IN = 0,
	CULL_OUT,
	CULL_INTERSECT
};

class ComponentCamera : public Component
{
public:

	ComponentCamera(GameObject* go = nullptr);
	~ComponentCamera();

	void Update(float dt);

	float* GetViewMatrix() const;

	float GetNear() const;
	float GetFar() const;
	float GetFov() const;
	float GetAspectRatio() const;

	void SetNear(float f_near);
	void SetFar(float f_far);
	void SetFov(float f_fov);
	void SetAspectRatio(float f_ratio);

	void DebugDraw();
	void DrawInspector();

	Culling ContainsAaBox(const math::AABB refBox) const;

public:

	math::Frustum frustum;
	bool culling = false;

};


#endif
