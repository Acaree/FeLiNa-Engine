#pragma once

#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

class Primitive
{
public:

	Primitive();

	virtual void	Render() const;
	virtual void	InnerRender() const;
	void			SetPos(float x, float y, float z);
	void			SetRotation(float angle, const float3 &u);
	void			Scale(float x, float y, float z);
	PrimitiveTypes	GetType() const;

public:
	
	Color color;
	float4x4 transform;
	bool axis,wire;

protected:
	PrimitiveTypes type;
};

// ============================================
class mCube : public Primitive
{
public :
	mCube();
	mCube(float sizeX, float sizeY, float sizeZ);
	void InnerRender() const;
public:
	float3 size;
};

// ============================================
class mSphere : public Primitive
{
public:
	mSphere();
	mSphere(float radius);
	//void InnerRender() const;
public:
	float radius;
};

// ============================================
class mCylinder : public Primitive
{
public:
	mCylinder();
	mCylinder(float radius, float height);
	void InnerRender() const;
public:
	float radius;
	float height;
	Cylinder* GeoLib_cylinder;
};

// ============================================
class mLine : public Primitive
{
public:
	mLine();
	mLine(float x, float y, float z);
	void InnerRender() const;
public:
	float3 origin;
	float3 destination;
};

// ============================================
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