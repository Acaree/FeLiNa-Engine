#ifndef _GEOMETRY_
#define _GEOMETRY_

#include "OpenGL.h"
#include "Globals.h"
#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"

enum GeometryType
{
Type_CubeDirectMode,
Type_CubeArrays,
Type_CubeArraysandIndex,
Type_Line
};

class Geometry
{
public:

	Geometry(float3 position, float3 size,GeometryType type);

	virtual void	Render();
	GeometryType	GetType();

	virtual ~Geometry();

public:

	Color color;
	float3 position;

protected:
	float3 size;
	GeometryType type;
};

#endif