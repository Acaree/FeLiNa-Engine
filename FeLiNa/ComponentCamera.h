#ifndef _COMPONENT_CAMERA_
#define _COMPONENT_CAMERA_

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/Geometry/Frustum.h"
#include "MathGeoLib/Geometry/AABB.h"

class ComponentCamera : public Component
{
public:

	ComponentCamera(GameObject* go = nullptr);
	~ComponentCamera();

	void Update(float dt);

	float* GetViewMatrix() const;
	float* GetProjectionMatrix() const;

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

	bool ContainsAaBox(const math::AABB refBox) const;

	void OnSave(JSON_Object* obj);
	void OnLoad(JSON_Object* obj);

public:

	math::Frustum frustum;
	bool culling = false;

};


#endif
