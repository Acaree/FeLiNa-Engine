#ifndef _COMPONENT_TRANSFORM
#define _COMPONENT_TRANSFORM

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class Component_Transform : public Component
{
public:
	Component_Transform(GameObject* parent);
	~Component_Transform();


	float3x3 GetTransformMatrix() const;
	
	void SetPosition(float3 new_pos);
	void SetRotation(float3 new_rotation);
	void SetScale(float3 new_scale);

	float3 GetPosition()const;
	float3 GetRotation()const;
	float3 GetScale()const;


private:

	float3 position;
	float3 euler_angles;
	float3 scale;

	float3x3 transform_matrix;


};

#endif
