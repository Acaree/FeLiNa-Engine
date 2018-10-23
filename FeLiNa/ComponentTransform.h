#ifndef _COMPONENTTRANSFORM
#define _COMPONENTTRANSFORM

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* parent);
	~ComponentTransform();


	float4x4 GetTransformMatrix() const;
	
	void SetPosition(float3 new_pos);
	void SetRotation(float3 new_rotation);
	void SetScale(float3 new_scale);

	float3 GetPosition()const;
	float3 GetRotation()const;
	float3 GetScale()const;


private:

	void UpdateMatrix();

	float3 position;
	float3 euler_angles;
	float3 scale;
	Quat quat_rotation;

	float4x4 transform_matrix;


};

#endif
