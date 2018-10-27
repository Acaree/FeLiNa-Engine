#ifndef _COMPONENT_CAMERA_
#define _COMPONENT_CAMERA_

#include "Globals.h"
#include "Component.h"
#include "MathGeoLib/Geometry/Frustum.h"

class ComponentCamera : public Component
{
public:

	ComponentCamera(GameObject* go = nullptr);
	~ComponentCamera();

	float GetNear() const;
	float GetFar() const;
	float GetFov() const;
	float GetAspectRatio() const;

	void SetNear(float f_near);
	void SetFar(float f_far);
	void SetFov(float f_fov);
	void SetAspectRatio(float f_ratio);

private:

	Frustum frustum;
	

};


#endif
