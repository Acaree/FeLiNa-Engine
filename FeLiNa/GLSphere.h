#ifndef _GLSPHERE_
#define _GLSPHERE_


#include "Geometry.h"

class GLSphere : public Geometry
{
public:

	GLSphere(float3 position, float3 size, GeometryType type);

	void Render();

private:

	uint index = 0;
	uint sindices = 1;

	uint sphere_id = 0;
	uint sphere_indices_id = 0;
	float radius_circle = 1.0F;
	float circle_angle = 30.0F;
	float3 vertex_circle[27];
	uint indices_circle[72];

};

#endif
