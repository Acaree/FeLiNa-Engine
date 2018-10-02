#include "OpenGL.h"
#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"

enum GeometryType
{
CubeDirectMode,
CubeArrays,
CubeArraysandIndex
};

class Geometry
{
public:

	Geometry(float3 position, int size,GeometryType type);

	virtual void	Render();
	GeometryType	GetType();

	virtual ~Geometry();

public:

	Color color;
	float3 position;

protected:
	int size;
	GeometryType type;
};

