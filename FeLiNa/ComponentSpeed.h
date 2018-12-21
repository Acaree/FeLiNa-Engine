#ifndef _COMPONENTSPEED
#define _COMPONENTSPEED

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentSpeed : public Component
{
public:
	ComponentSpeed(GameObject* parent, math::float3 speed = math::float3::zero);

	void DrawInspector();

	void OnSave(JSON_Object* obj);
	void OnLoad(JSON_Object* obj);
	void Update(float dt);
	void SetDirection(math::float3 new_direction);
	void SetVelocity(float velocity);
private:

	math::float3 direction = math::float3::zero;
	float velocity = 0.0f;

};

#endif
