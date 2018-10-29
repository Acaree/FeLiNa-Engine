#ifndef _COMPONENTTRANSFORM
#define _COMPONENTTRANSFORM

#include "Component.h"
#include "MathGeoLib/MathGeoLib.h"

class ComponentTransform : public Component
{
public:
	ComponentTransform(GameObject* parent, math::float3 position = math::float3::zero, math::float3 rotation = math::float3::zero, math::float3 scale = math::float3::one);
	~ComponentTransform();


	void DrawInspector();

	math::float4x4 GetTransformMatrix() const;

	void SetPosition(math::float3 new_pos);
	void SetRotation(math::float3 new_rotation);
	void SetScale(math::float3 new_scale);
	void SetQuaternion(math::Quat quaternion);

	void SumPosition(math::float3 new_pos);
	void SumRotation(math::float3 new_rotation);
	void SumScale(math::float3 new_scale);

	math::float3 GetPosition()const;
	math::float3 GetRotation()const;
	math::float3 GetScale()const;
	math::Quat GetQuaternion()const;

	void UpdateMatrix();

private:



	math::float3 position = math::float3::zero;
	math::float3 euler_angles = math::float3::zero;
	math::float3 scale = math::float3::one;
	math::Quat quat_rotation = math::Quat::identity;

	math::float4x4 local_matrix = math::float4x4::identity;
	math::float4x4 global_matrix = math::float4x4::identity;


};

#endif
