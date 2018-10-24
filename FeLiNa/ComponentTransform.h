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
	void SetQuaternion(Quat quaternion);

	float3 GetPosition()const;
	float3 GetRotation()const;
	float3 GetScale()const;
	Quat GetQuaternion()const;

private:

	void UpdateMatrix();

	float3 position = {0,0,0};
	float3 euler_angles = {0,0,0};
	float3 scale = {1,1,1};
	Quat quat_rotation = {0, 0, 0, 1};

	float4x4 transform_matrix;


};

#endif
