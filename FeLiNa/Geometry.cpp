#include "Geometry.h"


Geometry::Geometry(float3 position, int size, GeometryType type) : size(size), position(position), type(type)
{

}

Geometry::~Geometry()
{
	
}

void Geometry::Render() {
	
}

GeometryType Geometry::GetType() {

	return type;
}