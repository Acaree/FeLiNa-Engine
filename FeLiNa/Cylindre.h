#ifndef _CYLINDRE_
#define _CYLINDRE_


#include "Geometry.h"

class Cylindre : public Geometry
{
public:

	Cylindre(float3 position, float3 size, GeometryType type);

	void Render();

private:

	uint index = 0;
	uint sindices = 1;

	float cylinder_angle = 30.F;
	uint cylinder_id = 0;
	uint cylinder_indices_id = 0;
	float3 vertex_cylinder[28];
	uint indices_cylinder[145];

	float circle_angle = 30.0F;

	float radius_cylinder = 1.0F;
	float latitude = 0.5F;

	uint my_id = 0;
	uint my_indices = 0;
};

#endif

