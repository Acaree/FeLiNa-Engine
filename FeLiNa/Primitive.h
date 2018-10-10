#ifndef _PRIMITIVE_
#define _PRIMITIVE_

#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"


class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const float3 &u);
	void			Scale(float x, float y, float z);
	

public:
	
	Color color;
	float4x4 transform;
	bool axis,wire;

protected:
	
};


class mPlane : public Primitive
{
public:
	mPlane();
	mPlane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	//vec3 normal;
	//float constant;
	bool is_grid = false;
	Plane* matGeo_plane;
};

#endif